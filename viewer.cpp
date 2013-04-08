#include "sm.hpp"

#include "SFML/Graphics.hpp"//SFML 2.0

#include <fstream>
#include <iostream>
#include <cmath>

using namespace sm;

const float MAX_ZOOM=8.0f;
const float MIN_ZOOM=0.5f;
const float ZOOM_SPEED=1.5f;

void setupForRoom(sm::Room& room, sf::Texture& tilesTexture, sf::VertexArray& level, float& x, float& y){
	Array2D<Color> tilesBuffer;
	const unsigned tilesWide=32;
	room.drawTileSet(tilesBuffer, tilesWide);
	sf::Image tilesImage;
	tilesImage.create(tilesBuffer.readISize(), tilesBuffer.readJSize());
	for(unsigned i=0; i<tilesImage.getSize().x; ++i)
		for(unsigned j=0; j<tilesImage.getSize().y; ++j){
			Color c=tilesBuffer.at(i, j);
			tilesImage.setPixel(i, j, sf::Color(c.r*255, c.g*255, c.b*255, c.a*255));
		}
	tilesTexture.loadFromImage(tilesImage);
	std::vector<Vertex> vertices;
	room.getQuadsVertexArray(vertices, tilesWide);
	level.clear();
	level.setPrimitiveType(sf::Quads);
	for(unsigned i=0; i<vertices.size(); ++i)
		level.append(sf::Vertex(
			sf::Vector2f(vertices[i].x, vertices[i].y),
			sf::Vector2f(vertices[i].tx, vertices[i].ty)
		));
	x=room.readW()/2;
	y=room.readH()/2;
}

int main(int argc, char **argv){
	std::ofstream out("stdout.txt");
	std::ofstream err("stderr.txt");
	std::cout.rdbuf(out.rdbuf());
	std::cerr.rdbuf(err.rdbuf());
	sf::RenderWindow window(sf::VideoMode(640, 480, 32), "dan's super metroid viewer");
	Rom rom;
	rom.open("sm.smc");
	Room room;
	room.open(rom, sm::ROOM_OFFSETS[0]);
	sf::Texture tilesTexture;
	sf::VertexArray level;
	float x, y, w=window.getSize().x, h=window.getSize().y, zoom=2.0f;
	setupForRoom(room, tilesTexture, level, x, y);
	int previousMouseX=0, previousMouseY=0;
	bool dragging=false;
	while(true){
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
						viewPosition=window.convertCoords(sf::Vector2i(sfEvent.mouseButton.x, sfEvent.mouseButton.y));
						const sm::Door* door=room.readDoor(floor(viewPosition.x/TILE_SIZE), floor(viewPosition.y/TILE_SIZE));
						if(door&&door->room){
							room.open(rom, door->room);
							setupForRoom(room, tilesTexture, level, x, y);
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
						case sf::Keyboard::Space: x=room.readW()/2; y=room.readH()/2; break;
						case sf::Keyboard::Num1: room.open(rom, ROOM_OFFSETS[  0]); setupForRoom(room, tilesTexture, level, x, y); break;//crateria
						case sf::Keyboard::Num2: room.open(rom, ROOM_OFFSETS[ 46]); setupForRoom(room, tilesTexture, level, x, y); break;//brinstar
						case sf::Keyboard::Num3: room.open(rom, ROOM_OFFSETS[ 91]); setupForRoom(room, tilesTexture, level, x, y); break;//norfair
						case sf::Keyboard::Num4: room.open(rom, ROOM_OFFSETS[166]); setupForRoom(room, tilesTexture, level, x, y); break;//wrecked ship
						case sf::Keyboard::Num5: room.open(rom, ROOM_OFFSETS[200]); setupForRoom(room, tilesTexture, level, x, y); break;//maridia
						case sf::Keyboard::Num6: room.open(rom, ROOM_OFFSETS[237]); setupForRoom(room, tilesTexture, level, x, y); break;//tourian
						case sf::Keyboard::Num7: room.open(rom, ROOM_OFFSETS[256]); setupForRoom(room, tilesTexture, level, x, y); break;//ceres
						case sf::Keyboard::Num8: room.open(rom, ROOM_OFFSETS[262]); setupForRoom(room, tilesTexture, level, x, y); break;//debug
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
		if(!window.isOpen()) break;
		window.clear();
		window.setView(sf::View(sf::FloatRect(
			sf::Vector2f(x-w*zoom/2, y-h*zoom/2),
			sf::Vector2f(w*zoom, h*zoom)
		)));

#if 1
		level.clear();
		level.append(sf::Vertex(
			sf::Vector2f(0, 0),
			sf::Vector2f(0, 0)
		));
		level.append(sf::Vertex(
			sf::Vector2f(tilesTexture.getSize().x, 0),
			sf::Vector2f(tilesTexture.getSize().x, 0)
		));
		level.append(sf::Vertex(
			sf::Vector2f(tilesTexture.getSize().x, tilesTexture.getSize().y),
			sf::Vector2f(tilesTexture.getSize().x, tilesTexture.getSize().y)
		));
		level.append(sf::Vertex(
			sf::Vector2f(0, tilesTexture.getSize().y),
			sf::Vector2f(0, tilesTexture.getSize().y)
		));
#endif

		window.draw(level, sf::RenderStates(&tilesTexture));
		window.display();
		sf::sleep(sf::seconds(1/60.0f));
	}
}
