
#include "uci.h"

#include "position.h"
#include "search.h"

#include <fstream>
#include <iostream>
#include <sstream>

void position(std::istringstream& iss) {

  std::string fen, token;
  iss >> token;

  if (token == "startpos") {
    fen = STARTPOS;
    iss >> token;
  } else if (token == "current") {
    fen = Position::fen();
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
  std::string mode, arg;
  if (iss >> mode >> arg) {
    if (mode == "perft")
      Perft::go(std::stoi(arg));
    else if (mode == "bench")
      Perft::bench(std::stoi(arg));
    else if (mode == "play") {
      while (1) {
        system("clear");
        std::cout << Position::to_string();
        Position::do_commit(Search::best_move(std::stoi(arg)));
      }
    }
  }
  else {
    Move best_move = Search::best_move(2500);
    std::cout << "bestmove " << move_to_uci(best_move) << "\n";
  }
}

void UCI::loop() {

  system("clear");
  Position::set(STARTPOS);
  std::string cmd, token;
  
  do
  {
    std::getline(std::cin, cmd);
    std::istringstream iss(cmd);
    iss >> token;
    if (token == "position")
      position(iss);
    else if (token == "d")
      std::cout << Position::to_string();
    else if (token == "go")
      go(iss);
    else if (token == "view") {
      std::ifstream src("uci.cpp");
      while (std::getline(src, token))
	std::cout << token << "\n";
    }
      
    else if (cmd != "quit")
      std::cout
	<< "commands\n"
	<< "  go\n"
	<< "  go perft [depth]\n"
	<< "  go bench [depth]\n"
	<< "  go play [milliseconds]\n"
	<< "  d\n"
	<< "  position current <moves m1 m2 ... mn>\n"
	<< "  position startpos <moves m1 m2 ... mn>\n"
	<< "  position fen [fen] <moves m1 m2 ... mn>\n\n";
  } while (cmd != "quit");
}

