
#include "uci.h"

#include "position.h"

#include <iostream>
#include <sstream>

void position(std::istringstream& iss) {

  std::string fen, token;
  iss >> token;

  if (token == "startpos") {
    fen = STARTPOS;
    iss >> token;
  } else if (token == "fen") {
    while (iss >> token && token != "moves")
      fen += token + " ";
  }
  else
    return;
  
  Position::set(fen);
  
  while (iss >> token)
    Position::do_commit(uci_to_move(token));
}

void go(std::istringstream& iss) {
  std::string token, depth;
  iss >> token >> depth;
  if (token == "perft")
    Perft::go(std::stoi(depth));
  else if (token == "bench")
    Perft::bench(std::stoi(depth));
}

void UCI::loop() {

  Position::set(STARTPOS);
  std::cout << Position::to_string();
    
  std::string cmd, token;
  
  do
  {
    std::getline(std::cin, cmd);
    std::istringstream iss(cmd);
    iss >> token;
    if (token == "position")
      position(iss);
    if (token == "d")
      std::cout << Position::to_string();
    if (token == "go")
      go(iss);
  } while (cmd != "quit");
}

