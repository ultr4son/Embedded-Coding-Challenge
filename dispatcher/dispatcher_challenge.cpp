#include <iostream>
#include <map>
#include <string>
#include <iostream>
#include <functional>


#define ASSIGN_WITH_TYPE(type, accessor, object, name, value, valueT) _assign<type>(object, name, payload["value"].accessor, valueT);


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
		}
		return true;
	}

	bool flatten(rapidjson::Value& payload) {
		cout << "Controller::flatten: command: \n";
		if (payload.HasMember("tree") && payload["tree"].IsObject() && payload.HasMember("levels") && payload["levels"].IsInt()) {
			Value tree = payload["tree"].GetObject();
			int levels = payload["levels"].GetInt();
			Document d;
			MemoryPoolAllocator<CrtAllocator>& allocator = d.GetAllocator();

			Value& flattened = _flatten(tree, levels, allocator);

			StringBuffer buffer;
			Writer<StringBuffer> writer(buffer);
			d.Accept(writer);
			cout << buffer.GetString() << endl;
		}
		return true;
	}


	bool assign(rapidjson::Value& payload) {
		if (payload.HasMember("object") && payload["object"].IsObject() && payload.HasMember("name") && payload.IsString() && payload.HasMember("value")) {
			Value object = payload["object"].GetObject();
			string name = payload["name"].GetString();
			switch (payload.GetType()) {
			case kStringType: 
				_assign<string>(object, name, payload["value"].GetString(), kStringType);
				break;
			}
			_assign(object, name, )

			Document d;
			
			

			StringBuffer buffer;
			Writer<StringBuffer> writer(buffer);
			d.Accept(writer);
			cout << buffer.GetString() << endl;
		}

	}

private:
	template<typename T> 
	void _assign(Value& node, string name, T value, Type valueT) {
		for (auto& member : node.GetObject()) {
			if (member.name == name && member.value.GetType() == valueT) {
				member.value.Set<T>(value);
			}
			else if (member.value.GetType() == kObjectType) {
				_assign(member.value, name, value, valueT);
			}
		}
	}
	tuple<int, int> _find(Value& tree, string find, int depth) {
		int e = 0;
		for (auto& member : tree.GetObject()) {
			cout << "member: " << member.name.GetString() << endl;
			if (member.name.GetString() == find) {
				return std::make_tuple(depth, e);
			}
			Value leaf = member.value.GetObject();
			tuple<int, int> find_result = _find(leaf, find, depth + 1);
			if (get<0>(find_result) != -1 && get<1>(find_result) != -1) {
				return find_result;
			}
			e++;
		}

		return std::make_tuple(-1, -1);
	}

	Value& _flatten(Value& node, int depth, MemoryPoolAllocator<CrtAllocator>& allocator) {
		if (depth == 0) {
			return node;
		}
		if (node.GetType() == Type::kObjectType) {
			for (auto& m : node.GetObject()) {
				cout << m.name.GetString() << endl;
				if (m.value.GetType() == Type::kObjectType) {
					Value leaf = m.value.GetObject();
					Value& leaf_flat = _flatten(leaf, depth - 1, allocator);
					for (auto& f : leaf_flat.GetObject()) {
						node.AddMember(f.name, f.value, allocator);
					}
					node.RemoveMember(m.name);
				}

			}
		}

		return node;
		
	}
		// implement 3-4 more comands
};

// Bonus Question: why did I type cast this?
typedef std::function<bool(rapidjson::Value &)> CommandHandler;

class CommandDispatcher {
public:

	CommandDispatcher()
    {
    }

    virtual ~CommandDispatcher()
    {
		// question why is it virtual? Is it needed in this case?
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
	command_dispatcher.addCommandHandler("flatten", std::bind(&Controller::flatten, &controller, std::placeholders::_1));
	command_dispatcher.addCommandHandler("find", std::bind(&Controller::find, &controller, std::placeholders::_1));

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
