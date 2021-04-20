//
// Created by #Suyghur, on 4/20/21.
//

#include "time_kit.h"

std::string TimeKit::GetDate() {
    time_t now = time(nullptr);
    tm local_time = *localtime(&now);
    std::string *date;
    size_t buffer_size = sizeof(char) * 20;
    char *buffer = (char *) malloc(buffer_size);
    strftime(buffer, buffer_size, "%Y-%m-%d", &local_time);
    date = new std::string(buffer);
    free(buffer);
    return *date;
}
