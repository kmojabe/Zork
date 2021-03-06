/*
 * runGame.cpp
 *
 *  Created on: Nov 17, 2017
 *      Author: Abhir
 */

#include "runGame.h"
#include "rapidxml.hpp"

using namespace rapidxml;
using namespace std;

runGame::runGame() {
	// TODO Auto-generated constructor stub

}

runGame::~runGame() {
	// TODO Auto-generated destructor stub
}

void runGame::setStatus(bool curr_status){
	this->status = curr_status;
}

bool runGame::getStatus(){
	return this->status;
}

bool runGame::SetupGame(std::string fileName){
	std::ifstream file(fileName);

	if(!(file.is_open())){
		setStatus(false);
		cout << "File cannot open!!" << endl;
		return false;
 	}

	xml_document<> doc;
	xml_node<> * root_node;

	std::vector<char> buffer((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
	buffer.push_back('\0');
	doc.parse<0>(&buffer[0]);

	// Find root node
	root_node = doc.first_node();

	if (root_node == nullptr){
		setStatus(false);
	} else {
		setStatus(true);
	}

	xml_node<> * root;
	root = root_node->first_node();

	while(root != nullptr){
		if (string(root->name()) == "room"){
			//cout << root->name() << endl;
			Room* room = new Room(root->first_node());
			this->rooms[room->name] = room;
			//cout << " runGame:" << root->first_node()->value() <<endl;
		}
		else if (string(root->name()) == "item"){
			//cout << root->name() << endl;
			Item* item = new Item(root->first_node());
			this->items[item->name] = item;
		}
		else if (string(root->name()) == "container"){
			Container* container = new Container(root->first_node());
			this->containers[container->name] = container;
			//cout << root->name() << endl;
		}
		else if (string(root->name()) == "creature"){
			Creature* creature = new Creature(root->first_node());//root->first_node());
			this->creatures[creature->name] = creature; //[creature->name] creature;
		}
		root = root->next_sibling();
	}

	return getStatus();
}

void runGame::run(std::string fileName){
	bool setup = SetupGame(fileName);
	if (setup == false){
		return;
	}

	for (map<string,Room*>::iterator it = rooms.begin(); it!= rooms.end(); it++){
		//cout << it->second->name << endl;
		Room* roomNode;
		roomNode = it->second;
		if(! roomNode->items.empty()){
			for (map<string,Item*>::iterator it = roomNode->items.begin(); it!= roomNode->items.end(); it++){
				string itemName;
				itemName = it->first;
				it->second = this->items[itemName];
			}
		}
		if(! roomNode->containers.empty()){

			for (map<string,Container*>::iterator it = roomNode->containers.begin(); it!= roomNode->containers.end(); it++){
				string containerName;
				containerName = it->first;
				it->second = this->containers[containerName];
				//cout << it->second->name << endl;
			}
		}
		if(! roomNode->creatures.empty()){

			for (map<string,Creature*>::iterator it = roomNode->creatures.begin(); it!= roomNode->creatures.end(); it++){
				string creatureName;
				creatureName = it->first;
				it->second = this->creatures[creatureName];
			}
		}
	}

	for (map<string,Container*>::iterator it = containers.begin(); it!= containers.end(); it++){
		Container* containNode;
		containNode = it->second;
		if (! containNode->items.empty()){
			for (map<string,Item*>::iterator it = containNode->items.begin(); it!= containNode->items.end(); it++){
				it->second = this->items[it->first];
			}
		}
	}
	//continueRun();
}/*
void runGame::continueRun(){
	this->WhereAmI = "Entrance";
	cout << this->rooms[WhereAmI]->description << endl;
	bool TriggerExist;
	string command;
	cin >> command;
	cout << command << endl;
	TriggerExist = checkTrig(command);

}

bool runGame::checkTrig(string command){
	bool output;
	Room* currentRoom = this->rooms[this->WhereAmI];
	if (! currentRoom->triggers.empty()){
		//cout << currentRoom->triggers.front()->type << endl;
		output = navigateTrig(currentRoom->triggers, command);
	}

	//check the creatures in the room
	for (map<string,Creature*>::iterator it = currentRoom->creatures.begin(); it!= currentRoom->creatures.end(); it++){
		Creature * myCreature;
		myCreature = it->second;
		//cout << myCreature->name << endl;
		if (! myCreature->triggers.empty()){
			//cout << currentRoom->triggers.front()->type << endl;
			output = navigateTrig(myCreature->triggers, command);
		}
	}

	//check the items in the room
	for (map<string,Item*>::iterator it = currentRoom->items.begin(); it!= currentRoom->items.end(); it++){
		Item * myItem;
		myItem = it->second;
		//cout << myItem->name << endl;
		if (! myItem->triggers.empty()){
			//cout << currentRoom->triggers.front()->type << endl;
			output = navigateTrig(myItem->triggers, command);
		}
	}

	//check the items in the inventory
	for (map<string,Item*>::iterator it = this->inventory.begin(); it!= this->inventory.end(); it++){
		Item * myItem;
		myItem = it->second;
		//cout << myItem->name << endl;
		if (! myItem->triggers.empty()){
			//cout << currentRoom->triggers.front()->type << endl;
			output = navigateTrig(myItem->triggers, command);
		}
	}

	//navigate the containers within the room
	for (map<string,Container*>::iterator it = currentRoom->containers.begin(); it!= currentRoom->containers.end(); it++){
		Container* currContain;
		currContain = it->second;
		//go through each item within the container
		for (map<string,Item*>::iterator contain_it = currContain->items.begin(); contain_it != currContain->items.end(); contain_it++){
			Item* currItem;
			currItem = contain_it->second;
			//checks trigger vector within the items
			if (! currItem->triggers.empty()){
				//cout << currentRoom->triggers.front()->type << endl;
				output = navigateTrig(currItem->triggers, command);
			}
		}
		//check the triggers within the container
		if (! currContain->triggers.empty()){
			//cout << currentRoom->triggers.front()->type << endl;
			output = navigateTrig(currContain->triggers, command);
		}
	}
	return output;
}

bool runGame::navigateTrig(vector<Trigger*> triggers, string command){
	bool triggerValid;
	for (int i=0; i < triggers.size(); i++) {
		Trigger * currTrig;
		currTrig = triggers[i];
		//cout << currTrig->print << endl;
		if (command == currTrig->command){
			if (currTrig->type != "stop"){
				triggerValid = validConditions(currTrig->conditions);
				if (triggerValid){
					return true;
				}
			}
		}
	}
	return false;
}

bool runGame::validConditions(vector<Condition*> conditions){
	if (conditions.empty()){
		return false;
	}

	for (int i=0; i < conditions.size(); i++){
		Condition* curr_condition;
		curr_condition = conditions[i];
		if (curr_condition->type_has){
			//check if owner has the object.. either container, inventory, or room
			if (curr_condition->owner == "inventory"){
				bool ItemInInventory;
				bool has;
				cout << "I'm in validConditions" << endl;
				Item* foundItem;
				foundItem = FindItem(curr_condition->object);
				ItemInInventory = checkInventory(foundItem);
				has = checkHas(curr_condition->has);
				if (ItemInInventory and has){
					return true;
				} else if((not ItemInInventory) and (not has)){
					return true;
				} else{
					return false;
				}

			}

		}
	}
	return true;
}

Item* runGame::findItem(string itemName){
	Item* foundItem;
	for (map<string,Item*>::iterator it = items.begin(); it!= items.end(); it++){
		cout << "IN findItem" << endl;
		cout << it->first << endl;
		if (it->first == itemName){
			return it->second;
		}
	}
	return foundItem;
}

bool runGame::checkInventory(Item* object){
	if (this->inventory.empty()){
		return false;
	}
}

bool runGame::checkHas(string has){
	if (has == "no"){
		return false;
	} else{
		return true;
	}
}*/
/*
void runGame::command(string command, bool userInput) {
	stringstream strstr(command);
	istream_iterator<string> it(strstr);
	istream_iterator<string> end;
	vector<string> results(it, end);

	if (command.find("attack") != -1){
		attack(command);
	}
	else if (command.find("put") != -1){
		put(command);
	}
	else if (command.find("read") != -1){
		read(command);
	}
	else if (command.find("drop") != -1){
		drop(command);
	}
	else if (command.find("turn on") != -1){
		turnOn(command);
	}
	else if (command.find("open") != -1){
		open(command);
	}
	else if (command.find("take") != -1){
		take(command);
	}
	else if (command.find("open exit") != -1){
		bool exit;
		Room* currRoom = rooms[this -> location];
		if (1) {
			if (currRoom -> type == "exit"){
				cout << "Game Over" << endl;
				setStatus(false);
				exit = true;
			}
			else {
				cout << "This is not an exit!" << endl;
				exit = false;
			}
		}

		if (exit){
			return;
		}
	}
	else if (command == "i"){
		findInventory();
	}
	else if ((command == "n") || (command == "north")){
		direction("n");
	}
	else if ((command == "s") || (command == "south")){
		direction("s");
	}
	else if ((command == "w") || (command == "west")){
		direction("w");
	}
	else if ((command == "e") || (command == "east")){
		direction("e");
	}
	else if ((command.find("Add")) != -1 && !userInput){
		add(command);
	}
	else if ((command.find("Update")) != -1 && !userInput){
		update(command);
	}
	else if ((command.find("Delete")) != -1 && !userInput){
		del(command);
	}
	else if ((command.find("Game Over")) != -1 && !userInput){
		cout << "Victory!" << endl;
		setStatus(false);
	}
}*/
/*bool Game::execTrigger(vector<Trigger*> triggers, string trigCommand){

	for(int i = 0; i < triggers.size(); i++){

		//Check for command
		if(trigCommand == triggers[i]->command || triggers[i]->command == ""){

			//Check not done
			if(triggers[i]->type != "done"){

				if(checkCondition(triggers[i]->conditions)){

					switch (triggers[i]->type) {
					case "single":
						triggers[i]->type = "done";
					}
					switch (triggers[i]->action.empty()) {
					case 0:
						for(int j = 0; j < triggers[i]->action.size(); j++){
							executeCommand(triggers[i]->action[j], false);
						}
					}
					if(triggers[i]->print != "")
						cout << triggers[i]->print << endl;
					return checkCondition(triggers[i]->conditions);
				}
			}
		}
		*else if(triggers[i]->command == ""){
			//Check not done
			if(triggers[i]->type != "done"){
				//result = checkCondition(c_trigger->conditions);

				if(checkCondition(triggers[i]->conditions)){
					if(triggers[i]->type == "single"){
						triggers[i]->type = "done";
					}
					if(triggers[i]->print != "")
						cout << triggers[i]->print << endl;

					if(!(triggers[i]->action.empty())){
						for(int j = 0; j < c_trigger->action.size(); j++){
							executeCommand(c_trigger->action[j], false);
						}
					}
					return checkCondition(c_trigger->conditions);
				}
			}
		}*
	}
	return false;
}*/
/*void Game::direction(string dir){
	Room* cRoom = rooms[this -> location];
	map<string, Border*>::iterator it;

	switch (dir) {
	case "n":
		it = cRoom->borders.find("north");
		break;
	case "s":
		it = cRoom->borders.find("south");
		break;
	case "w":
		it = cRoom->borders.find("west");
		break;
	case "e":
		it = cRoom->borders.find("east");
	}
	*if (dir == "n"){
		it = cRoom -> borders.find("north");
	}
	else if (dir == "s"){
		it = cRoom -> borders.find("south");
	}
	else if (dir == "w"){
		it = cRoom -> borders.find("west");
	}
	else if (dir == "e"){
		it = cRoom -> borders.find("east");
	}*

	if (it == cRoom -> borders.end()){
		cout << "Can't go that way." << endl;
	}
	else {
		this -> location = it -> second -> name;
		SetupRoom(location);
		cout << rooms[location] -> description << endl;
	}
}*/
/*void Game::findInventory(){
	if (!inventory.empty()){
		cout << "Inventory: ";
		for (map<string, Item*>::iterator it = inventory.begin(); it != inventory.end(); it += 1) {
			if (it == inventory.begin()) {
				cout << it -> second -> name;
			}
			else {
				cout << ", " << it -> second -> name;
			}
		}
		cout << endl;
	}
	else {
		cout << "Inventory: empty" << std::endl;
	}
}*/
/*void Game::take(string command){
	vector<string> com = splitCommand(command);
	Room* cRoom = rooms[location];
	Container* cContainer;

	if (com.size() < 2){
		cout << "To take an item, the command must be in the form of 'take (item)'." << endl;
		return;
	}

	string item = com[1];
}*/
/*bool Game::read(string command){
	vector<string> com = splitCommand(command);
	if (com.size() < 2){
		cout << "Error! The read command should have the following format: 'read (item)'" << endl;
		return;
	}
	string item = com[1];

	if (inventory.find(item) == inventory.end()) {
		cout << "Error" << endl;
	}
	else {
		if (inventory[item]->writing == "") {
			cout << "Nothing written" << endl;
		}
		else {
			cout << inventory[item]->writing << endl;
		}
	}
}*/
/*void Game::turnOn(string command){
	vector<string> com = splitCommand(command);
	if (com.size() < 3) {
		cout << "Error! The turn on function command must have the following format: 'turn on (item)'" << endl;
	}
	//string item = com[2];
	else if (inventory.find(com[2]) == inventory.end() || inventory[com[2]] -> turnon == nullptr || inventory[com[2]]->turnon->action == "") {
		cout << "Error" << endl
	}
	else {
		cout << "You turned on the " << com[2] << endl << inventory[com[2]]->turnon->print << endl;
		executeCommand(inventory[com[2]]->turnon->action, false);
	}
}*/
/*void Game::drop(string command){
	vector<string> com = splitCommand(command);
	if (com.size() < 2){
		cout << "Error! The drop command should have the following format: 'drop (item)'" << endl;
		return;
	}
	string item = com[1];

	if (inventory.find(item) == inventory.end()) {
		cout << "Error" << endl;
	}
	else {
		rooms[location]->items[item] = inventory[item];
		inventory.erase(item);
		cout << item << " dropped" << endl;
	}
}*/
/*void Game::attack(string command){
	vector<string> com = splitCommand(command);

	if(com.size() < 4){
		cout << "Error! Attack command should be in the format: 'attack (creature) with (item)'" << endl;
	}

	//string creature = com[1];
	//string item = com[3];

	else if(inventory.find(com[3]) == inventory.end()){
		cout << "Error" << endl;
		//return;
	}

	//Creature is in game
	else if(creatures.find(com[1]) != creatures.end()){
		//Find vulnerability
		if((creatures[com[1]]->vulnerability.find(com[3]) != creatures[com[1]]->vulnerability.end()) ||  (creatures[com[1]]->vulnerability.size() == 0)){
			cout << "You assault " << com[1] << " with " << com[3] << "." << endl;

			if(creatures[com[1]]->attack != nullptr){
				//bool result = checkCondition(creatures[com[1]]->attack->conditions);

				//Conditions met
				if (checkCondition(creatures[com[1]]->attack->conditions)) {
					cout << creatures[com[1]]->attack->print << endl;

					for (int i = 0;  i < creatures[com[1]]->attack->actions.size(); i++){
						if((creatures[com[1]]->attack->actions[i].find("Delete") != -1) && (creatures[com[1]]->attack->actions[i].find(creatures[com[1]]->name) != -1)){
							executeCommand(creatures[com[1]]->attack->actions[i], false);
							break;
						}
						else {
							executeCommand(creatures[com[1]]->attack->actions[i], false);
						}
					}
					checkTriggers("");
					return;
				}
				else{
					cout << "Error" << endl;
				}
			}

		}
		else
			cout << "Error" << endl;
	}
	else {
		cout << "Error" << endl;
	}
}*/

// HELPER FUNCTIONS
/*vector<string> Game::splitCommand(string command){
	string buffer;
	stringstream ss(command);

	vector<string> words;

	while (ss >> buffer) {
		words.push_back(buffer);
	}

	return words;
}*/
/*bool Game::checkCondition(vector<Condition*> conditions){
	if (conditions.empty()){
		return true;
	}

	for (int i = 0; i < conditions.size(); i += 1){
		if (checkHas(conditions[i])) {
			if (!((hasOwner(conditions[i]) == 't' || hasOwner(conditions[i]) == 'n') && (hasStatus(conditions[i]) == 't' || hasStatus(conditions[i]) == 'n'))) {
				result = false;
			}
		}
		else {
			if(hasOwner(conditions[i]) == 't' || hasStatus(conditions[i]) == 't'){
				result = false;
			}
		}
	}
}*/

