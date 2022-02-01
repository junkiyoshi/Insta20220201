#include "ofApp.h"	

//--------------------------------------------------------------
Actor::Actor(vector<glm::vec3>& location_list, vector<vector<int>>& next_index_list, vector<int>& destination_list) {

	this->select_index = ofRandom(location_list.size());
	while (true) {

		auto itr = find(destination_list.begin(), destination_list.end(), this->select_index);
		if (itr == destination_list.end()) {

			destination_list.push_back(this->select_index);
			break;
		}

		this->select_index = (this->select_index + 1) % location_list.size();
	}

	this->next_index = this->select_index;

	this->color.setHsb(ofRandom(255), 150, 255);
}

//--------------------------------------------------------------
void Actor::update(const int& frame_span, vector<glm::vec3>& location_list, vector<vector<int>>& next_index_list, vector<int>& destination_list) {

	if (ofGetFrameNum() % frame_span == 0) {

		auto tmp_index = this->select_index;
		this->select_index = this->next_index;
		int retry = next_index_list[this->select_index].size() + 1;
		this->next_index = next_index_list[this->select_index][(int)ofRandom(next_index_list[this->select_index].size())];
		while (--retry > 0) {

			auto destination_itr = find(destination_list.begin(), destination_list.end(), this->next_index);
			if (destination_itr == destination_list.end()) {

				if (tmp_index != this->next_index) {

					destination_list.push_back(this->next_index);
					break;
				}
			}

			this->next_index = next_index_list[this->select_index][(this->next_index + 1) % next_index_list[this->select_index].size()];
		}
		if (retry <= 0) {

			destination_list.push_back(this->select_index);
			this->next_index = this->select_index;
		}
	}

	auto param = ofGetFrameNum() % frame_span;
	auto distance = location_list[this->next_index] - location_list[this->select_index];
	if (this->next_index != this->select_index) {

		this->prev_location = location;
	}
	this->location = location_list[this->select_index] + distance / frame_span * param;
}

//--------------------------------------------------------------
glm::vec3 Actor::get_location() {

	return this->location;
}

//--------------------------------------------------------------
glm::vec3 Actor::get_prev_location() {

	return this->prev_location;
}

//--------------------------------------------------------------
ofColor Actor::get_color() {

	return this->color;
}

//--------------------------------------------------------------
void ofApp::setup() {

	ofSetFrameRate(60);
	ofSetWindowTitle("openFrameworks");

	ofBackground(0);
	ofEnableDepthTest();

	this->line.setMode(ofPrimitiveMode::OF_PRIMITIVE_LINES);

	auto ico_sphere = ofIcoSpherePrimitive(250, 3);
	auto triangles = ico_sphere.getMesh().getUniqueFaces();
	for (auto& triangle : triangles) {

		auto avg = glm::vec3(triangle.getVertex(0) + triangle.getVertex(1) + triangle.getVertex(2)) / 3;
		this->location_list.push_back(avg);

		auto alpha = 255;
		if (avg.z < 0) {

			alpha = ofMap(avg.z, 0, -250, 255, 0);
		}

		this->line.addVertex(avg);
		this->line.addColor(ofColor(200, alpha));
	}

	int i = 0;
	for (auto& location : this->location_list) {

		vector<int> next_index = vector<int>();
		int index = -1;
		for (auto& other : this->location_list) {

			index++;
			if (location == other) { continue; }

			float distance = glm::distance(location, other);
			if (distance <= 25) {

				next_index.push_back(index);

				this->line.addIndex(i);
				this->line.addIndex(index);
			}
		}

		this->next_index_list.push_back(next_index);

		i++;
	}

	for (int i = 0; i < this->location_list.size() - 150; i++) {

		this->actor_list.push_back(make_unique<Actor>(this->location_list, this->next_index_list, this->destination_list));
	}
}

//--------------------------------------------------------------
void ofApp::update() {

	int frame_span = 15;
	int prev_index_size = 0;

	if (ofGetFrameNum() % frame_span == 0) {

		prev_index_size = this->destination_list.size();
	}

	for (auto& actor : this->actor_list) {

		actor->update(frame_span, this->location_list, this->next_index_list, this->destination_list);
	}

	if (prev_index_size != 0) {

		this->destination_list.erase(this->destination_list.begin(), this->destination_list.begin() + prev_index_size);
	}
}

//--------------------------------------------------------------
void ofApp::draw() {

	this->cam.begin();

	for (auto& actor : this->actor_list) {

		auto alpha = 255;
		if (actor->get_location().z < 0) {

			alpha = ofMap(actor->get_location().z, 0, -250, 255, 0);
		}

		ofSetColor(actor->get_color(), alpha);
		ofDrawSphere(actor->get_location(), 5);
	}

	this->line.drawWireframe();

	this->cam.end();
}

//--------------------------------------------------------------
int main() {

	ofSetupOpenGL(720, 720, OF_WINDOW);
	ofRunApp(new ofApp());
}