#include "sm.hpp"

#include "SFML/Graphics.hpp"//SFML 2.0

#include <fstream>
#include <iostream>
#include <cmath>

using namespace sm;

const float MAX_ZOOM=8.0f;
const float MIN_ZOOM=0.5f;
const float ZOOM_SPEED=1.5f;
const unsigned TILES_WIDE=32;

void updateTiles(sm::Room& room, sf::VertexArray& level){
	std::vector<Vertex> vertices;
	room.getQuadsVertexArray(vertices, TILES_WIDE);
	level.clear();
	level.setPrimitiveType(sf::Quads);
	for(unsigned i=0; i<vertices.size(); ++i)
		level.append(sf::Vertex(
			sf::Vector2f(vertices[i].x, vertices[i].y),
			sf::Vector2f(vertices[i].tx, vertices[i].ty)
		));
}

void setupRoom(sm::Room& room, U32 offset, sf::Texture& tilesTexture, sf::VertexArray& level, float& x, float& y){
	room.open(offset);
	room.loadGraphics();
	Array2D<Color> tilesBuffer;
	room.drawTileSet(tilesBuffer, TILES_WIDE);
	sf::Image tilesImage;
	tilesImage.create(tilesBuffer.readISize(), tilesBuffer.readJSize());
	for(unsigned i=0; i<tilesImage.getSize().x; ++i)
		for(unsigned j=0; j<tilesImage.getSize().y; ++j){
			Color c=tilesBuffer.at(i, j);
			tilesImage.setPixel(i, j, sf::Color(c.r*255, c.g*255, c.b*255, c.a*255));
		}
	tilesTexture.loadFromImage(tilesImage);
	updateTiles(room, level);
	x=room.readW()/2;
	y=room.readH()/2;
}

int main(int argc, char **argv){
	std::ofstream out("stdout.txt");
	std::ofstream err("stderr.txt");
	std::streambuf* coutbuf=std::cout.rdbuf();
	std::streambuf* cerrbuf=std::cerr.rdbuf();
	std::cout.rdbuf(out.rdbuf());
	std::cerr.rdbuf(err.rdbuf());
	sf::RenderWindow window(sf::VideoMode(640, 480, 32), "dan's super metroid viewer");
	Rom rom;
	rom.open("sm.smc");
	Room room(rom);
	U32 offset=sm::ROOM_OFFSETS[0];
	sf::Texture tilesTexture;
	sf::VertexArray level;
	float x, y, w=window.getSize().x, h=window.getSize().y, zoom=2.0f;
	setupRoom(room, offset, tilesTexture, level, x, y);
	Tile tile;
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
						const sm::Door* door=room.readDoor(viewPosition.x, viewPosition.y);
						if(door&&door->room){
							offset=door->room;
							setupRoom(room, offset, tilesTexture, level, x, y);
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
						case sf::Keyboard::C:{
							sf::Vector2f viewPosition;
							viewPosition=window.convertCoords(sf::Mouse::getPosition(window));
							tile=room.copy(viewPosition.x, viewPosition.y);
							break;
						}
						case sf::Keyboard::P:{
							sf::Vector2f viewPosition;
							viewPosition=window.convertCoords(sf::Mouse::getPosition(window));
							room.paste(tile, viewPosition.x, viewPosition.y);
							updateTiles(room, level);
							break;
						}
						case sf::Keyboard::S:
							if(room.save(offset)) rom.save("edited.smc");
							break;
						case sf::Keyboard::Space: x=room.readW()/2; y=room.readH()/2; break;
						case sf::Keyboard::Num1: setupRoom(room, offset=ROOM_OFFSETS[  0], tilesTexture, level, x, y); break;//crateria
						case sf::Keyboard::Num2: setupRoom(room, offset=ROOM_OFFSETS[ 46], tilesTexture, level, x, y); break;//brinstar
						case sf::Keyboard::Num3: setupRoom(room, offset=ROOM_OFFSETS[ 91], tilesTexture, level, x, y); break;//norfair
						case sf::Keyboard::Num4: setupRoom(room, offset=ROOM_OFFSETS[166], tilesTexture, level, x, y); break;//wrecked ship
						case sf::Keyboard::Num5: setupRoom(room, offset=ROOM_OFFSETS[200], tilesTexture, level, x, y); break;//maridia
						case sf::Keyboard::Num6: setupRoom(room, offset=ROOM_OFFSETS[237], tilesTexture, level, x, y); break;//tourian
						case sf::Keyboard::Num7: setupRoom(room, offset=ROOM_OFFSETS[256], tilesTexture, level, x, y); break;//ceres
						case sf::Keyboard::Num8: setupRoom(room, offset=ROOM_OFFSETS[262], tilesTexture, level, x, y); break;//debug
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
		window.draw(level, sf::RenderStates(&tilesTexture));
		window.display();
		sf::sleep(sf::seconds(1/60.0f));
	}
	std::cout<<std::flush;
	std::cerr<<std::flush;
	std::cout.rdbuf(coutbuf);
	std::cerr.rdbuf(cerrbuf);
}
