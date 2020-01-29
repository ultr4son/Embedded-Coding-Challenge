#include <iostream>
#include <map>
#include <string>
#include <iostream>
#include <functional>
#include <cmath>



//
// supporting tools and software
//
// Validate and test your json commands
// https://jsonlint.com/

// RapidJSON : lots and lots of examples to help you use it properly
// https://github.com/Tencent/rapidjson
//

// std::function
// std::bind
// std::placeholders
// std::map
// std::make_pair

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

using namespace rapidjson;
using namespace std;

bool g_done = false;

//
// TEST COMMANDS
//
auto help_command = R"(
 {
  "command":"help",
  "payload": {
    "usage":"Enter json command in 'command':'<command>','payload': { // json payload of arguments }",
  }
 }
)";

auto exit_command = R"(
 {
  "command":"exit",
  "payload": {
     "reason":"Exiting program on user request."
  }
 }
)";

auto add_command = R"( 
 {
  "command": "add",
  "payload": {
    "a": 5.2,
    "b": 2.8934
  }
}
)";

auto fizzbuzz_command = R"( 
 {
  "command": "fizzbuzz",
  "payload": {
    "start": 0,
    "end": 100,
    "fizz_div": 5,
    "buzz_div": 3
  }
}
)";

auto find_command = R"(
{
  "command": "find",
  "payload": {
    "tree": { 
			"wow": {
				"cool": {},
				"neat": {}
			}
		},
		"find": "neat"
  }
}
)";

auto assign_command = R"(
	{
  "command": "assign",
  "payload": {
    "object": {
      "test_1": 1,
      "test_2": "hi",
      "test_3": {
        "test_2": 3
      }
      
    },
    "name": "test_2",
    "value": 3
    }
  }

)";
class Controller {
public:
    bool help(rapidjson::Value &payload)
    {
        cout << "Controller::help: command: ";

		if (payload.HasMember("usage") && payload["usage"].IsString()) {
			cout << payload["usage"].GetString() << endl;
		}
		else {
			return false;
		}

        return true;
    }

    bool exit(rapidjson::Value &payload)
    {
        cout << "Controller::exit: command: \n";

		if (payload.HasMember("reason") && payload["reason"].IsString()) {
			cout << payload["reason"].GetString() << endl;
			g_done = true;
		}
		else {
			return false;
		}

        return true;
    }
	/*
		payload: {
			a: double,
			b: double
		}
		Add two doubles a and b.
	*/
	bool add(rapidjson::Value& payload) {
		cout << "Controller::add: command: \n";
		if (payload.HasMember("a") && payload["a"].IsDouble() && payload.HasMember("b") && payload["b"].IsDouble()) {
				double a = payload["a"].GetDouble();
				double b = payload["b"].GetDouble();

				cout << a + b << endl;
				
		}
		else {
			return false;
		}
		return true;
	}
	/*
		payload: {
			start: int,
			end: int,
			fizz_div: int,
			buzz_div: int
		}
		For i in start to end, 
		if i is divisible by fizz_div, print "fizz", 
		if i is divisible by buzz_div, print "buzz", 
		if i is divisible by both, print "fizzbuzz",
		else print nothing.
	*/
	bool fizzbuzz(rapidjson::Value& payload) {
		cout << "Controller::fizzbuzz: command: \n";
		if (payload.HasMember("start") && payload["start"].IsInt() 
			&& payload.HasMember("end") && payload["end"].IsInt() 
			&& payload.HasMember("fizz_div") && payload["fizz_div"].IsInt() 
			&& payload.HasMember("buzz_div") && payload["buzz_div"].IsInt()) {

			int start = payload["start"].GetInt();
			int end = payload["end"].GetInt();
			int fizz_div = payload["fizz_div"].GetInt();
			int buzz_div = payload["buzz_div"].GetInt();

			for (int i = start; i < end; i++) {
				cout << i << ":";
				if (i % fizz_div == 0) {
					cout << "fizz";
				}
				if (i % buzz_div == 0) {
					cout << "buzz";
				}
				cout << endl;

			}

		}
		else {
			return false;
		}
		return true;
	}
	/*
		payload: {
			tree: object,
			find: string
		}
		Gives the (depth, branch) location of the key find in the object tree.
		For example, for the object:
		{ 
			"wow": {
				"cool": {},
				"neat": {}
			}
		}
		find = "neat" will give the position 1, 1
		Expects tree to be composed of only objects.
	*/
	bool find(rapidjson::Value& payload) {
		cout << "Controller::find: command: \n";
		if (payload.HasMember("tree") && payload["tree"].IsObject() && payload.HasMember("find") && payload["find"].IsString()) {

			Value tree = payload["tree"].GetObject();
			string find = payload["find"].GetString();
			
			tuple<int, int> find_result = _find(tree, find, 0);

			if (get<0>(find_result) != -1 && get<1>(find_result) != -1) {
				cout << find << " was found at depth " << get<0>(find_result) << ", element " << get<1>(find_result) << endl;
			}
			else {
				cout << find << " is not is tree." << endl;
			}
			return true;

		}
		else {
			return false;
		}
	}

	/*
		payload: {
			object: object,
			name: string,
			value: any
		}
		Replaces all values in object with the key name and the same type as value with value.
	*/
	bool assign(rapidjson::Value& payload) {
		cout << "Controller::assign: command: \n";
		if (payload.HasMember("object") && payload["object"].IsObject() && payload.HasMember("name") && payload["name"].IsString() && payload.HasMember("value")) {
			Value object = payload["object"].GetObject();
			string name = payload["name"].GetString();

			//For setting values in object
			Document d;
			MemoryPoolAllocator<CrtAllocator>& allocator = d.GetAllocator();


			_assign(object, name, payload["value"], allocator);

			//Print object
			StringBuffer buffer;
			Writer<StringBuffer> writer(buffer);
			object.Accept(writer);
			cout << buffer.GetString() << endl;

			return true;

		}
		else {
			return false;
		}
	}

private:
	/*
		Recursive helper function to assign.
	*/
	void _assign(Value& node, string name, Value& value, MemoryPoolAllocator<>& allocator) {
		
		//Find value with same name and type as name and value
		Value::ConstMemberIterator itr = node.FindMember(name.c_str());
		if (itr != node.MemberEnd() && node[itr->name.GetString()].GetType() == value.GetType()) {
			node[itr->name.GetString()] = value;
		}

		//Recursively go through object
		for (auto& member : node.GetObject()) {
			if (member.value.GetType() == kObjectType) {
				_assign(member.value, name, value, allocator);
			}
		}
	}

	/*
		Recursive helper function to find. Expects tree to be object.
	*/
	tuple<int, int> _find(Value& tree, string find, int depth) {
		int e = 0;
		for (auto& member : tree.GetObject()) {
			
			//If member found, get location.
			if (member.name.GetString() == find) {
				return std::make_tuple(depth, e);
			}

			//Go through each leaf.
			Value leaf = member.value.GetObject();
			tuple<int, int> find_result = _find(leaf, find, depth + 1);
			if (get<0>(find_result) != -1 && get<1>(find_result) != -1) {
				return find_result;
			}
			e++;
		}

		return std::make_tuple(-1, -1);
	}

};

// Bonus Question: why did I type cast this?
// Doing a typedef of std::function<bool(rapidjson::Value &)> to CommandHandler gives a clearer purpose to what std::function<bool(rapidjson::Value &)> refers to in the code.
// This also makes it less time consuming and error prone to write std::function<bool(rapidjson::Value &)>.
// If using CommandHandler in other parts of the code, it will be more intuitive to the developer that what they are using relates to the CommandDispatcher and eliminates the chance of redefining what a CommandHandler is by writing std::function<bool(rapidjson::Value &)> wrong.

typedef std::function<bool(rapidjson::Value &)> CommandHandler;

class CommandDispatcher {
public:

	CommandDispatcher()
    {
    }

    virtual ~CommandDispatcher()
    {
		// question why is it virtual? Is it needed in this case?
		// Virtual destructors are used when it is expected that a derived class will be instantiated as a pointer to the base class's type.
		// This may be useful if there is a different impementation of dispatching desired (such as multithreading the dispatched commands).
	}

    bool addCommandHandler(std::string command, CommandHandler handler)
    {
        cout << "CommandDispatcher: addCommandHandler: " << command << std::endl;

		command_handlers_[command] = handler;

        return true;
    }

    bool dispatchCommand(std::string command_json)
    {
        cout << "COMMAND: " << command_json << endl;
		
		Document d;

		d.Parse(command_json.c_str());

		if (d.HasParseError()) {
			cout << d.GetParseError() << endl;
			return false;
		}
		
		//Assert that input must have members "command" and "payload" that are string and object respectively
		if (d.HasMember("command") && d.HasMember("payload") && 
			d["command"].IsString() && d["payload"].IsObject()) {
			
			bool success;
			//Attempt to invoke command
			try {
				success = command_handlers_.at(d["command"].GetString())(d["payload"]);
			}
			catch (const out_of_range& err) {
				std::cout << "Command : " << d["command"].GetString() << " not found" << endl;
				return false;
			}
			return success;

		}
		else {
			return false;
		}


        return true;
    }

private:
    std::map<std::string, CommandHandler> command_handlers_;

    // Question: why delete these?
	// Deleting these constructors makes the class non-copyable.
	// This prevents the class from being allocated 
	// If the dispatcher had a non-sharable resource, like a lock, we would not want to have it be copied to other dispatchers.
	// delete unused constructors
    CommandDispatcher (const CommandDispatcher&) = delete;
    CommandDispatcher& operator= (const CommandDispatcher&) = delete;

};

int main()
{
    std::cout << "COMMAND DISPATCHER: STARTED" << std::endl;

    CommandDispatcher command_dispatcher;
    Controller controller;                 // controller class of functions to "dispatch" from Command Dispatcher

    // Implement
    // add command handlers in Controller class to CommandDispatcher using addCommandHandler
	
	command_dispatcher.addCommandHandler("exit", std::bind(&Controller::exit, &controller, std::placeholders::_1));
	command_dispatcher.addCommandHandler("help", std::bind(&Controller::help, &controller, std::placeholders::_1));
	command_dispatcher.addCommandHandler("add", std::bind(&Controller::add, &controller, std::placeholders::_1));
	command_dispatcher.addCommandHandler("find", std::bind(&Controller::find, &controller, std::placeholders::_1));
	command_dispatcher.addCommandHandler("assign", std::bind(&Controller::assign, &controller, std::placeholders::_1));
	command_dispatcher.addCommandHandler("fizzbuzz", std::bind(&Controller::fizzbuzz, &controller, std::placeholders::_1));

    // command line interface for testing
    string command;
    while( ! g_done ) {
        cout << "COMMANDS: {\"command\":\"exit\", \"payload\":{\"reason\":\"User requested exit.\"}}\n";
        cout << "\tenter command : ";
        getline(cin, command);
        bool success = command_dispatcher.dispatchCommand(command);
		if (!success) {
			cout << "Unable to parse command." << endl;
		}
    }

    std::cout << "COMMAND DISPATCHER: ENDED" << std::endl;
    return 0;
}
