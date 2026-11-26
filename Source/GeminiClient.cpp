#include "GeminiClient.h"

#if JUCE_WINDOWS
 #include <windows.h>
 #include <winhttp.h>
 #include <wincred.h>
#endif

namespace
{
#if JUCE_WINDOWS
    constexpr wchar_t credentialTarget[] = L"R2R_AI_ASSISTANT_GEMINI_API_KEY";

    juce::String readCredential()
    {
        PCREDENTIALW credential = nullptr;
        if (!CredReadW(credentialTarget, CRED_TYPE_GENERIC, 0, &credential) || credential == nullptr)
            return {};
        const auto value = juce::String::fromUTF8(
            reinterpret_cast<const char*>(credential->CredentialBlob),
            static_cast<int>(credential->CredentialBlobSize));
        CredFree(credential);
        return value;
    }

    bool writeCredential(const juce::String& key)
    {
        const auto utf8 = key.toUTF8();
        CREDENTIALW credential {};
        credential.Type = CRED_TYPE_GENERIC;
        credential.TargetName = const_cast<LPWSTR>(credentialTarget);
        credential.UserName = const_cast<LPWSTR>(L"R2R AI Assistant");
        credential.Persist = CRED_PERSIST_LOCAL_MACHINE;
        credential.CredentialBlob = reinterpret_cast<LPBYTE>(const_cast<char*>(utf8.getAddress()));
        credential.CredentialBlobSize = static_cast<DWORD>(utf8.sizeInBytes() - 1);
        return CredWriteW(&credential, 0) == TRUE;
    }

    juce::String performGeminiRequest(const juce::String& prompt, const juce::String& key)
    {
        HINTERNET session = WinHttpOpen(L"R2R-AI-Assistant/2.0",
                                        WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY,
                                        WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
        if (session == nullptr)
            return "Connection error: Windows could not start HTTPS networking.";

        WinHttpSetTimeouts(session, 10000, 10000, 20000, 30000);
        HINTERNET connection = WinHttpConnect(session, L"generativelanguage.googleapis.com",
                                               INTERNET_DEFAULT_HTTPS_PORT, 0);
        if (connection == nullptr)
        {
            WinHttpCloseHandle(session);
            return "Connection error: could not reach Google's Gemini server.";
        }

        HINTERNET request = WinHttpOpenRequest(connection, L"POST",
            L"/v1beta/interactions", nullptr,
            WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);

        if (request == nullptr)
        {
            WinHttpCloseHandle(connection);
            WinHttpCloseHandle(session);
            return "Connection error: could not create the HTTPS request.";
        }

        const auto keyUtf8 = key.toUTF8();
        const auto headers = juce::String("Content-Type: application/json\r\n")
                           + "x-goog-api-key: " + key + "\r\n";
        const auto headersWide = headers.toWideCharPointer();
        const auto json = juce::JSON::toString(juce::var(new juce::DynamicObject()), true);
        juce::ignoreUnused(json);

        auto* bodyObject = new juce::DynamicObject();
        bodyObject->setProperty("model", "gemini-3.8-flash");
        bodyObject->setProperty("input", prompt);
        const auto body = juce::JSON::toString(juce::var(bodyObject));
        const auto bodyUtf8 = body.toUTF8();

        const BOOL sent = WinHttpSendRequest(request, headersWide, static_cast<DWORD>(-1L),
            const_cast<char*>(bodyUtf8.getAddress()), static_cast<DWORD>(bodyUtf8.sizeInBytes() - 1),
            static_cast<DWORD>(bodyUtf8.sizeInBytes() - 1), 0);
        if (!sent || !WinHttpReceiveResponse(request, nullptr))
        {
            WinHttpCloseHandle(request);
            WinHttpCloseHandle(connection);
            WinHttpCloseHandle(session);
            return "Network error: Gemini request failed. Check your internet connection.";
        }

        DWORD statusCode = 0, statusSize = sizeof(statusCode);
        WinHttpQueryHeaders(request, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                            WINHTTP_HEADER_NAME_BY_INDEX, &statusCode, &statusSize,
                            WINHTTP_NO_HEADER_INDEX);

        juce::MemoryBlock response;
        DWORD available = 0;
        while (WinHttpQueryDataAvailable(request, &available) && available > 0)
        {
            juce::HeapBlock<char> chunk(static_cast<size_t>(available) + 1);
            DWORD downloaded = 0;
            if (!WinHttpReadData(request, chunk.getData(), available, &downloaded) || downloaded == 0)
                break;
            response.append(chunk.getData(), downloaded);
        }

        WinHttpCloseHandle(request);
        WinHttpCloseHandle(connection);
        WinHttpCloseHandle(session);

        const auto responseText = juce::String::fromUTF8(
            static_cast<const char*>(response.getData()), static_cast<int>(response.getSize()));
        const auto parsed = juce::JSON::parse(responseText);

        if (statusCode < 200 || statusCode >= 300)
        {
            if (auto* object = parsed.getDynamicObject())
            {
                const auto error = object->getProperty("error");
                if (auto* errorObject = error.getDynamicObject())
                    return "Gemini API error (" + juce::String((int) statusCode) + "): "
                         + errorObject->getProperty("message").toString();
            }
            return "Gemini API returned HTTP " + juce::String((int) statusCode)
                 + ". Check the API key and Gemini API access.";
        }

        if (auto* root = parsed.getDynamicObject())
        {
            const auto stepsValue = root->getProperty("steps");
            if (auto* stepArray = stepsValue.getArray())
            {
                juce::String answer;
                for (const auto& step : *stepArray)
                {
                    if (auto* stepObject = step.getDynamicObject())
                    {
                        if (stepObject->getProperty("type").toString() != "model_output")
                            continue;
                        const auto contentValue = stepObject->getProperty("content");
                        if (auto* contentArray = contentValue.getArray())
                        {
                            for (const auto& item : *contentArray)
                            {
                                if (auto* itemObject = item.getDynamicObject())
                                    if (itemObject->getProperty("type").toString() == "text")
                                        answer += itemObject->getProperty("text").toString();
                            }
                        }
                    }
                }
                if (answer.isNotEmpty())
                    return answer;
            }
        }
        return "Gemini returned no text. Try a shorter prompt.";
    }
#endif
}

GeminiClient::GeminiClient() : Thread("R2R Gemini request") {}

GeminiClient::~GeminiClient()
{
    stopThread(3000);
}

bool GeminiClient::saveApiKey(const juce::String& key)
{
#if JUCE_WINDOWS
    const auto trimmed = key.trim();
    return trimmed.isNotEmpty() && writeCredential(trimmed);
#else
    juce::ignoreUnused(key);
    return false;
#endif
}

juce::String GeminiClient::loadApiKey() const
{
#if JUCE_WINDOWS
    return readCredential();
#else
    return {};
#endif
}

void GeminiClient::clearApiKey()
{
#if JUCE_WINDOWS
    CredDeleteW(credentialTarget, CRED_TYPE_GENERIC, 0);
#endif
}

bool GeminiClient::ask(const juce::String& prompt, const juce::String& apiKey, Reply reply)
{
    if (isThreadRunning() || prompt.trim().isEmpty() || apiKey.trim().isEmpty())
        return false;

    {
        const juce::ScopedLock lock(requestLock);
        pendingPrompt = prompt.trim();
        pendingKey = apiKey.trim();
        pendingReply = std::move(reply);
    }
    return startThread();
}

void GeminiClient::run()
{
    juce::String prompt, key;
    Reply reply;
    {
        const juce::ScopedLock lock(requestLock);
        prompt = pendingPrompt;
        key = pendingKey;
        reply = pendingReply;
    }

    juce::String result;
#if JUCE_WINDOWS
    result = performGeminiRequest(prompt, key);
#else
    result = "Gemini networking is currently implemented for Windows builds.";
#endif

    if (reply)
        juce::MessageManager::callAsync([callback = std::move(reply), result]() { callback(result); });

    {
        const juce::ScopedLock lock(requestLock);
        pendingKey.clear();
        pendingPrompt.clear();
        pendingReply = {};
    }
}
