#include <iostream>
#include <string>
#include <regex>
#include <curl/curl.h>

// Функция обратного вызова для обработки данных, полученных от curl
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Функция для поиска адресов электронной почты
void findEmails(const std::string& htmlContent) {
    std::regex emailRegex(R"((\w+[\.-]?\w+@\w+[\.-]?\w+(\.\w+)+))");
    auto wordsBegin = std::sregex_iterator(htmlContent.begin(), htmlContent.end(), emailRegex);
    auto wordsEnd = std::sregex_iterator();

    std::cout << "Найденные адреса электронной почты:\n";
    for (std::sregex_iterator i = wordsBegin; i != wordsEnd; ++i) {
        std::cout << i->str() << std::endl;
    }
}

int main() {
    std::string domain;
    std::cout << "Введите домен (например, example.com): ";
    std::cin >> domain;

    std::string url = "http://" + domain; // Составляем URL
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0");

        // Выполнение запроса
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }
        else {
            findEmails(readBuffer); // Поиск адресов электронной почты
        }

        // Освобождение ресурсов
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    return 0;
}
