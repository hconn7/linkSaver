#include <algorithm>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

namespace settings {
std::string fileName{"links.txt"};
inline int ID{};
} // namespace settings

struct Entry {
  int id;
  std::string name;
  std::string link;
  std::string description;

  bool operator==(const Entry& e) const = default;
};
struct Commands {
  std::string name;
  std::string description;
  std::vector<Entry> entries;
};

std::ostream& operator<<(std::ostream& out, const Entry& l) {
  return out << l.id << '\t' << l.name << '\t' << l.link << '\t'
             << l.description << '\n';
}
std::ostream& operator<<(std::ostream& out, const Commands& c) {
  return out << c.name << " " << c.description << '\n';
}
const std::vector<Commands> e{
    Commands{"[L]ist links", "Get all links with all Data"},
    Commands{"[N]ame of Links", "Get all names of the current links"},
    Commands{"[D]escriptions",
             "Get all link descriptions with its corresponding name"}};

void allCommands() {
  for (const auto& cmd : e) {
    std::cout << " Name: " << cmd.name << " Description: " << cmd.description
              << '\n';
  }
};
bool readFile() {
  if (std::filesystem::exists("links.txt")) {
    std::runtime_error("Failed to load existing data");
  }
  return true;
}

std::vector<Entry> loadEntries() {
  std::vector<Entry> entries{};

  if (readFile()) {
    std::ifstream f{"links.txt"};
    std::string line{};

    while (std::getline(f, line)) {
      Entry entry{};
      if (line.empty()) {
        continue;
      }
      std::istringstream ss(line);
      ss >> entry.id;
      ss.ignore();
      std::getline(ss, entry.name, '\t');
      std::getline(ss, entry.link, '\t');
      std::getline(ss, entry.description, '\t');
      entries.push_back(entry);
    }
  }
  return entries;
}
int openBrower(std::string l) {
  std::string command = "xdg-open " + l + " > /dev/null 2>&1";
  int result{std::system(command.c_str())};
  return result;
}
void listLinks() {
  std::ifstream f{"links.txt"};
  std::vector<Entry> entries{loadEntries()};
  for (const auto& e : entries) {
    std::cout << "ID: " << e.id << " Name: " << e.name << " Link: " << e.link
              << " Description: " << '\n';
  }
  while (true) {
    int c{};
    std::cout << "Type a link ID to go the link directly!\n";
    std::cin >> c;
    std::cin.ignore();

    auto entry = std::find_if(entries.begin(), entries.end(),
                              [c](const Entry e) { return e.id == c; });

    if (entry->id > 0) {
      openBrower(entry->link);
      std::cout << "Your link is open!\n";
    } else {
      std::cout << "Make sure you hit the correct id!";
    }
  }
}

void getCommands(char c) {
  switch (c) {
  case 'l':
    listLinks();
    break;
  case 'n':
    std::cout << e[1];
    break;
  case 'd':
    std::cout << e[2];
    break;
  default:
    std::cout << "Command not found";
    break;
  }
}

Entry findEntry(std::vector<Entry> arr, std::ostream, std::string name) {
  for (const auto& a : arr) {
    if (a.name == name) {
      return Entry{a.id, a.name, a.link, a.description};
    }
  }
  std::cerr << "Entry not found";
  return Entry{};
}

int writeToIDNumberFile() {
  if (std::filesystem::exists("TotalLinks.txt") &&
      std::filesystem::file_size("TotalLinks.txt") > 0) {
    std::ifstream inf{"TotalLinks.txt"};
    inf >> settings::ID;
  }

  std::ofstream outf{"TotalLinks.txt"};
  settings::ID++;

  if (outf) {
    outf << settings::ID;
  }

  return settings::ID;
}

void ignoreLine() {
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}
bool clearFailedExtraction() {
  if (!std::cin) {

    std::cin.clear();
    ignoreLine();
    return true;
  }
  return false;
}
Entry handleMultipleArgs(int id, int argc, char* argv[]) {
  if (argc > 1) {
    std::ofstream outf{"links.txt", std::ios::app};
    if (!outf) {
      std::ofstream outf = std::ofstream{settings::fileName};
    }

    if (argc == 2) {
      char* link{argv[1]};
      Entry e{id, "Name", link, "no description"};
      outf << e;
      return e;
    }
    if (argc == 3) {
      char* link{argv[1]};
      std::string name{argv[2]};
      if (name == "go") {
        openBrower(link);
      } else {
        Entry e{id, name, link, " no description"};
        outf << e;
        return e;
      }
    }
    if (argc > 3) {
      char* name{argv[1]};
      char* link{argv[2]};
      std::vector<const char*> desc(argv + 3, argv + argc);
      std::string description{""};
      for (const auto& d : desc) {
        description += d;
        description += " ";
      }
      Entry e{id, name, link, description};
      outf << e;
      return e;
    }
  }
  return Entry{};
}

int main(int argc, char* argv[]) {
  if (argc > 1) {
    int id{writeToIDNumberFile()};
    Entry e{handleMultipleArgs(id, argc, argv)};
    if (e == Entry{}) {
      std::cerr << "Error handling arguments";
      return 1;
    }
  } else {
    std::cout
        << "\nTo enter a link type its Name(optional), URL, and an "
           "(optional)description in terminal. \n"
           "e.g ./mybinary learcpp https://www.learncpp.com learncpp link\n";

    std::cout << "Click [h]elp for commands\n";
    while (true) {
      char input{};
      std::cin >> input;
      if (clearFailedExtraction()) {
        std::cout << "That input was invalid\n";
      }
      switch (input) {
      case 'h':
        allCommands();
        break;
      case 'l':
        getCommands(input);
        break;
      case 'n':
        getCommands(input);
        break;
      case 'd':
        getCommands(input);
        break;
      }
    }
  }
}
