#include "sm.hpp"

#include <SFML/Graphics.hpp>

#include <fstream>
#include <iostream>
#include <cmath>
#include <sstream>

using namespace sm;

const float MAX_ZOOM=8.0f;
const float MIN_ZOOM=0.5f;
const float ZOOM_SPEED=1.5f;
const unsigned TILES_WIDE=32;

template <class T> std::string toString(T t){
	std::stringstream ss;
	ss<<t;
	return ss.str();
}

sf::Vector2f sfVector2f(int x, int y){
	return sf::Vector2f((float)x, (float)y);
}

sf::Color sfColor(float r, float g, float b, float a){
	return sf::Color(sf::Uint8(255*r), sf::Uint8(255*g), sf::Uint8(255*b), sf::Uint8(255*a));
}

void updateTiles(sm::Room& room, sf::VertexArray& level, bool layer1, bool layer2, bool mode7){
	std::vector<Vertex> vertices;
	room.getQuadsVertexArray(vertices, TILES_WIDE, layer1, layer2, mode7);
	level.clear();
	level.setPrimitiveType(sf::Quads);
	for(unsigned i=0; i<vertices.size(); ++i)
		level.append(sf::Vertex(
			sfVector2f(vertices[i].x, vertices[i].y),
			sfVector2f(vertices[i].tx, vertices[i].ty)
		));
}

void drawTexture(const sf::Texture& texture, sf::VertexArray& vertices){
	vertices.clear();
	vertices.append(sf::Vertex(
		sfVector2f(0, 0),
		sfVector2f(0, 0)
	));
	vertices.append(sf::Vertex(
		sfVector2f(texture.getSize().x, 0),
		sfVector2f(texture.getSize().x, 0)
	));
	vertices.append(sf::Vertex(
		sfVector2f(texture.getSize().x, texture.getSize().y),
		sfVector2f(texture.getSize().x, texture.getSize().y)
	));
	vertices.append(sf::Vertex(
		sfVector2f(0, texture.getSize().y),
		sfVector2f(0, texture.getSize().y)
	));
}

void setupRoom(sm::Room& room, U16 index, int& state, sf::Texture& tilesTexture, sf::VertexArray& level, float& x, float& y, bool layer1, bool layer2, bool mode7, bool standardState){
	room.open(sm::VANILLA_ROOM_OFFSETS[index]);
	if(standardState) state=room.readStates()-1;
	room.setState(state);
	room.loadGraphics();
	Array2D<Color> tilesBuffer;
	room.drawTileSet(tilesBuffer, TILES_WIDE);
	sf::Image tilesImage;
	tilesImage.create(tilesBuffer.readISize(), tilesBuffer.readJSize());
	for(unsigned i=0; i<tilesImage.getSize().x; ++i)
		for(unsigned j=0; j<tilesImage.getSize().y; ++j){
			Color c=tilesBuffer.at(i, j);
			tilesImage.setPixel(i, j, sfColor(c.r, c.g, c.b, c.a));
		}
	tilesTexture.loadFromImage(tilesImage);
	updateTiles(room, level, layer1, layer2, mode7);
	x=room.readPixelsWide()/2.0f;
	y=room.readPixelsHigh()/2.0f;
	if(standardState) state=room.readStates()-1;
	else if(state>=(int)room.readStates()) state=room.readStates()-1;
}

int main(int argc, char **argv){
	//sfml
	sf::RenderWindow window(sf::VideoMode(640, 480, 32), "super metroid viewer");
	sf::Text text;
	text.setColor(sf::Color(255, 0, 0, 255));
	text.setScale(0.5f, 0.5f);
	//sm
	Rom rom;
	{
		std::string s=rom.open("sm.smc");
		if(s!=""){
			std::cerr<<s<<'\n';
			return -1;
		}
	}
	if(!rom.indexVanilla()) return -1;
	Room room(rom);
	//state initialization
	U16 index=0;
	int state;
	sf::Texture tilesTexture;
	sf::VertexArray level;
	float x, y, w=(float)window.getSize().x, h=(float)window.getSize().y, zoom=2.0f;
	Tile tile;
	int previousMouseX=0, previousMouseY=0;
	bool dragging=false, layer1=true, layer2=true, mode7=true, tileSet=false;
	setupRoom(room, index, state, tilesTexture, level, x, y, layer1, layer2, mode7, true);
	//loop
	while(true){
		//handle events
		window.setView(sf::View(sf::FloatRect(
			sf::Vector2f(x-w*zoom/2, y-h*zoom/2),
			sf::Vector2f(w*zoom, h*zoom)
		)));
		sf::Event sfEvent;
		while(window.pollEvent(sfEvent)){
			switch(sfEvent.type){
				case sf::Event::MouseMoved:
					if(dragging){
						x-=zoom*(sfEvent.mouseMove.x-previousMouseX);
						y-=zoom*(sfEvent.mouseMove.y-previousMouseY);
						previousMouseX=sfEvent.mouseMove.x;
						previousMouseY=sfEvent.mouseMove.y;
					}
					break;
				case sf::Event::MouseWheelMoved:
					if(sfEvent.mouseWheel.delta>0) zoom/=sfEvent.mouseWheel.delta*ZOOM_SPEED;
					else zoom*=-sfEvent.mouseWheel.delta*ZOOM_SPEED;
					if(zoom>MAX_ZOOM) zoom=MAX_ZOOM;
					else if(zoom<MIN_ZOOM) zoom=MIN_ZOOM;
					break;
				case sf::Event::MouseButtonPressed:
					if(sfEvent.mouseButton.button==sf::Mouse::Left){
						previousMouseX=sfEvent.mouseButton.x;
						previousMouseY=sfEvent.mouseButton.y;
						dragging=true;
					}
					else if(sfEvent.mouseButton.button==sf::Mouse::Right){
						sf::Vector2f viewPosition;
						viewPosition=window.mapPixelToCoords(sf::Vector2i(sfEvent.mouseButton.x, sfEvent.mouseButton.y));
						sm::Transition door(rom);
						if(room.readDoorWithPixelPosition((unsigned)viewPosition.x, (unsigned)viewPosition.y, door)&&door.room){
							for(index=0; index<sm::VANILLA_ROOMS; ++index)
								if(sm::VANILLA_ROOM_OFFSETS[index]==door.room)
									break;
							setupRoom(room, index, state, tilesTexture, level, x, y, layer1, layer2, mode7, true);
						}
					}
					break;
				case sf::Event::MouseButtonReleased:
					if(sfEvent.mouseButton.button==sf::Mouse::Left) dragging=false;
					break;
				case sf::Event::KeyPressed:
					switch(sfEvent.key.code){
						case sf::Keyboard::Q: window.close(); break;
						case sf::Keyboard::M: zoom=zoom>=MAX_ZOOM?MAX_ZOOM:zoom*ZOOM_SPEED; break;
						case sf::Keyboard::N: zoom=zoom<=MIN_ZOOM?MIN_ZOOM:zoom/ZOOM_SPEED; break;
						case sf::Keyboard::B: zoom=2; break;
						case sf::Keyboard::V: zoom=1; break;
						case sf::Keyboard::Space: x=room.readPixelsWide()/2.0f; y=room.readPixelsHigh()/2.0f; break;
						case sf::Keyboard::Num1: setupRoom(room, index=  0, state, tilesTexture, level, x, y, layer1, layer2, mode7, true); break;//crateria
						case sf::Keyboard::Num2: setupRoom(room, index= 46, state, tilesTexture, level, x, y, layer1, layer2, mode7, true); break;//brinstar
						case sf::Keyboard::Num3: setupRoom(room, index= 91, state, tilesTexture, level, x, y, layer1, layer2, mode7, true); break;//norfair
						case sf::Keyboard::Num4: setupRoom(room, index=166, state, tilesTexture, level, x, y, layer1, layer2, mode7, true); break;//wrecked ship
						case sf::Keyboard::Num5: setupRoom(room, index=200, state, tilesTexture, level, x, y, layer1, layer2, mode7, true); break;//maridia
						case sf::Keyboard::Num6: setupRoom(room, index=237, state, tilesTexture, level, x, y, layer1, layer2, mode7, true); break;//tourian
						case sf::Keyboard::Num7: setupRoom(room, index=256, state, tilesTexture, level, x, y, layer1, layer2, mode7, true); break;//ceres
						case sf::Keyboard::Num8: setupRoom(room, index=262, state, tilesTexture, level, x, y, layer1, layer2, mode7, true); break;//debug
						case sf::Keyboard::Numpad0:
							tileSet=!tileSet;
							if(tileSet) drawTexture(tilesTexture, level);
							else updateTiles(room, level, layer1, layer2, mode7);
							break;
						case sf::Keyboard::Numpad1: layer1=!layer1; updateTiles(room, level, layer1, layer2, mode7); break;
						case sf::Keyboard::Numpad2: layer2=!layer2; updateTiles(room, level, layer1, layer2, mode7); break;
						case sf::Keyboard::Numpad3: mode7 =!mode7 ; updateTiles(room, level, layer1, layer2, mode7); break;
						case sf::Keyboard::Left:
							if(index>0){
								--index;
								setupRoom(room, index, state, tilesTexture, level, x, y, layer1, layer2, mode7, true);
							}
							break;
						case sf::Keyboard::Right:
							if(index<sm::VANILLA_ROOMS-1){
								++index;
								setupRoom(room, index, state, tilesTexture, level, x, y, layer1, layer2, mode7, true);
							}
							break;
						case sf::Keyboard::Up:
							if(state<(int)room.readStates()-1){
								++state;
								setupRoom(room, index, state, tilesTexture, level, x, y, layer1, layer2, mode7, false);
							}
							break;
						case sf::Keyboard::Down:
							if(state>0){
								--state;
								setupRoom(room, index, state, tilesTexture, level, x, y, layer1, layer2, mode7, false);
							}
							break;
						default: break;
					}
					break;
				case sf::Event::Resized:
					w=float(sfEvent.size.width);
					h=float(sfEvent.size.height);
					break;
				case sf::Event::Closed:
					window.close();
					break;
				default: break;
			}
		}
		//draw
		if(!window.isOpen()) break;
		window.clear();
		window.draw(level, sf::RenderStates(&tilesTexture));
		window.setView(sf::View(sf::FloatRect(
			sf::Vector2f(0.0f, 0.0f),
			sf::Vector2f(w, h)
		)));
		std::string s;
		s+="room: "+toString(index)+"."+toString(state)+"\n";
		s+="state description: "+sm::Header::codeDescription(room.readStateCode(state))+"\n";
		text.setString(s);
		window.draw(text);
		window.display();
		sf::sleep(sf::seconds(1/60.0f));
	}
	//finish
	return 0;
}
