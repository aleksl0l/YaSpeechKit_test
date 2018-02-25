#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <time.h>
#include <curl/curl.h>

size_t write_response_data(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	std::stringstream* s = (std::stringstream*)userdata;
	size_t n = size * nmemb;
	s->write(ptr, n);
	return n;
}

size_t read_request_data(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	std::ifstream* f = (std::ifstream*)userdata;
	size_t n = size * nmemb;  
	f->read(ptr, n);
	size_t result = f->gcount();  
	return result;
}

void gen_random(std::string &s, const int len) {
	static const char alphanum[] =
		"0123456789"
		//"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdef";//ghijklmnopqrstuvwxyz";

	for (int i = 0; i < len; ++i) {
		s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
	}

	s[len] = 0;
}

const char* FILE_PATH = "./untitled.wav";

int main()
{
	srand(time(NULL));
	CURL *curl = NULL;
	curl = curl_easy_init();
	if (curl)
	{
		curl_easy_setopt(curl, CURLOPT_HEADER, 1);
		curl_easy_setopt(curl, CURLOPT_POST, 1);
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);

		struct curl_slist *headers = NULL;

		headers = curl_slist_append(headers, "Content-Type: audio/x-wav");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

		curl_easy_setopt(curl, CURLOPT_USERAGENT, "Dalvik/1.2.0 (Linux; U; Android 2.2.2; LG-P990 Build/FRG83G");
		std::string uiid(32, 0);
		gen_random(uiid, 32);
		std::string key("your_key");

		std::string req("asr.yandex.net/asr_xml?key=");
		req += key + "&uuid=" + uiid + "&topic=queries&lang=ru-RU";
		std::cout << req.c_str();

		curl_easy_setopt(curl, CURLOPT_URL, req.c_str());

		std::ifstream fileStream(FILE_PATH, std::ifstream::binary);
		fileStream.seekg(0, fileStream.end);
		int length = fileStream.tellg();
		fileStream.seekg(0, fileStream.beg);

		curl_easy_setopt(curl, CURLOPT_READFUNCTION, &read_request_data);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, length);
		curl_easy_setopt(curl, CURLOPT_READDATA, &fileStream);

		std::stringstream contentStream;

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &write_response_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &contentStream);

		CURLcode code = curl_easy_perform(curl);

		unsigned httpCode;
		curl_easy_getinfo(curl, CURLINFO_HTTP_CODE, &httpCode);
		std::stringstream msg;
		msg << "Http code is " << httpCode;

		std::fstream file;
		file.open("recogn.xml", std::fstream::out);
		file << contentStream.str();
		file.close();

		curl_free(headers);
		curl_easy_cleanup(curl);
	}
	return 0;
}