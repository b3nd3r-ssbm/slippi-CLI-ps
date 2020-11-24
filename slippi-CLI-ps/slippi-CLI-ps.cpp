#include <filesystem>
#include <iostream>
#include <fstream>
#include <xstring>
#include <string>
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

string readPartFile(string fileName) {
    ifstream ifs(fileName);
    string content((std::istreambuf_iterator<char>(ifs)),
        (std::istreambuf_iterator<char>()));
    return content;
}

string convert_ASCII(string hex) {
    string ascii = "";
    for (size_t i = 0; i < hex.length(); i += 2) {
        //taking two characters from hex string
        string part = hex.substr(i, 2);
        //changing it into base 16
        char ch = stoul(part, nullptr, 16);
        //putting it into the ASCII string
        ascii += ch;
    }
    return ascii;
}

string readFile(const string& fileName)
{
    ifstream ifs(fileName.c_str(), ios::in | ios::binary | ios::ate);

    ifstream::pos_type fileSize = ifs.tellg();
    ifs.seekg(0, ios::beg);

    vector<char> bytes(fileSize);
    ifs.read(bytes.data(), fileSize);

    return string(bytes.data(), fileSize);
}

string shortenFileRead(string path) {
    string fileRead = readFile(path);
    return fileRead.substr(fileRead.find("clipSU"));
}

string doubleBackSlash(string data) { 
    for (int i = 0; i < data.length(); i++) {
        if (data[i] == "\\"[0]) {
            data = data.substr(0, i) + "\\" + data.substr(i);
            i++;
        }
    }
    return data;
}

void playFile(string fileRead, string path) {
    string tempPath = filesystem::temp_directory_path().string() + "\\slippi-CLI-ps.json";
    string slippiPath = tempPath.substr(0, tempPath.find("Local")) + "Roaming\\Slippi Desktop App\\";
    string dolphinPath = slippiPath + "dolphin\\Dolphin.exe";
    dolphinPath = "\"" + dolphinPath + "\"";
    path = doubleBackSlash(path);
    string temp = fileRead.substr(fileRead.find("clipStartSU") + 12);
    int start = stoi(temp.substr(0, temp.find("U")));
    int length = temp.find("clipEndSU") + 10;
    length = temp.find("}") - length;
    int end = stoi(temp.substr(temp.find("clipEndSU") + 10, length));
    string fileWrite = "{\"outputOverlayFiles\":false,\"isRealTimeMode\":false,\"commandId\":\"0\",\"mode\":\"normal\",\"replay\":\"" + path + "\",\"startFrame\":" + to_string(start) + ",\"endFrame\":" + to_string(end) + "}";
    ofstream writer(tempPath);
    writer << fileWrite;
    writer.close();
    string isoPath = readFile(slippiPath + "Settings");
    isoPath = isoPath.substr(isoPath.find("isoPath\":\"") + 10);
    isoPath = isoPath.substr(0, isoPath.find("\""));
    string commandS = "\"" + dolphinPath + " -i \"" + tempPath + "\" -e \"" + isoPath + "\"\"";
    const char* command = commandS.c_str();
    system(command);
    //cout << command;
}

string indivFile(string path) {
    string fileRead = readFile(path);
    path = doubleBackSlash(path);
    string temp = fileRead.substr(fileRead.find("clipStartSU") + 12);
    int start = stoi(temp.substr(0, temp.find("U")));
    int length = temp.find("clipEndSU") + 10;
    length = temp.find("}") - length;
    int end = stoi(temp.substr(temp.find("clipEndSU") + 10, length));
    string gameJson = "{\"path\":\"" + path + "\",\"startFrame\":" + to_string(start) + ",\"endFrame\":" + to_string(end) + ",\"gameStartAt\":\"\",\"gameStation\":\"\"}";
    return gameJson;
}

void playFiles(char** files, int listLength) {
    string tempPath = filesystem::temp_directory_path().string() + "\\slippi-CLI-ps.json";
    string slippiPath = tempPath.substr(0, tempPath.find("Local")) + "Roaming\\Slippi Desktop App\\";
    string dolphinPath = slippiPath + "dolphin\\Dolphin.exe";
    dolphinPath = "\"" + dolphinPath + "\"";
    string isoPath = readFile(slippiPath + "Settings");
    isoPath = isoPath.substr(isoPath.find("isoPath\":\"") + 10);
    isoPath = isoPath.substr(0, isoPath.find("\""));
    string commandS = "\"" + dolphinPath + " -i \"" + tempPath + "\" -e \"" + isoPath + "\"\"";
    const char* command = commandS.c_str();
    string fileWrite = "{\"outputOverlayFiles\":false,\"isRealTimeMode\":false,\"commandId\":\"0\",\"mode\":\"queue\",\"queue\":[";
    fileWrite += indivFile(files[1]);
    for (int i = 2; i < listLength; i++) {
        fileWrite += ",";
        fileWrite += indivFile(files[i]);
    }
    fileWrite += "]}";
    ofstream writer(tempPath);
    writer << fileWrite;
    writer.close();
    system(command);
}

int countCommas(string str) {
    int i = 0;
    while (str.find(",") != -1) {
        str = str.substr(str.find(","));
        i++;
    }
    return i;
}

string fileExt(string file) {
    string end = file.substr(file.length() - 5);
    if (end == ".sclp") {
        return "sclp";
    }
    if (end == ".json") {
        return "json";
    }
    if (end.substr(1) == ".slp") {
        return "slp";
    }
    return "none";
}

void error() {
    cout << "The purpose of this app is to provide a much smaller way to watch .sclp files.\n";
    cerr << "Error: no compatible parameters (files) passed, please pass sclp files via command line or using Open With\n";
    cerr << "Make sure that all parameters passed are of the same file type!\n";
    cout << "Please close this window";
    string keepOpen;
    cin >> keepOpen;
}

int main(int argc, char** argv) {
    if (argc > 1 && fileExt(argv[1]) == "sclp") {
        if (argc == 2) {
            string fileRead = shortenFileRead(argv[1]);
            playFile(fileRead, argv[1]);
            return 0;
        }
        playFiles(argv, argc);
    }
    else {
        error();
    }
}

