#ifndef BUTTON_CPP
#define BUTTON_CPP

#include <SFML/Graphics.hpp>
#include <iostream>

namespace gui {

	namespace state {
		enum {
			normal = 0,
			hovered = 1,
			clicked = 2
		};
	}

	class button {
	public:
		button() {}

		button(sf::Text text, sf::Font font, sf::Vector2f position) {
			_position = position;
			_bg_color = sf::Color::White;

			_text = text;
			_scale = sf::Vector2f(_text.getGlobalBounds().width + 30.f, _text.getGlobalBounds().height + 20.f);

			_button = sf::RectangleShape(_scale);
			_button.setOrigin(_button.getGlobalBounds().width / 2, _button.getGlobalBounds().height / 2);
			_button.setPosition(_position);
			_button.setFillColor(_bg_color);
			_button_state = state::normal;

			center_text();
		}

		void set_text(sf::Text text) { _text = text; }
		void set_text(std::string s) { _text.setString(s); }
		void set_bg_color(sf::Color color) { _bg_color = color; }
		void set_border_color(sf::Color color) { _border_color = color; }
		void set_position(sf::Vector2f pos) { 
			_position = pos;
			_button.setPosition(_position);
			center_text();
		}
		void set_scale(sf::Vector2f scale) { 
			_scale = scale;
			_button.setScale(_scale);
		}
		void center_text() {
			_text.setOrigin(_text.getGlobalBounds().width / 2, _text.getGlobalBounds().height / 2);
			_button.setOrigin(_button.getGlobalBounds().width / 2, _button.getGlobalBounds().height / 2);
			_text.setPosition(_button.getPosition().x, _button.getPosition().y - _text.getGlobalBounds().height / 4);
		}

		void update(sf::Event& e, sf::RenderWindow& window) {

			_button = sf::RectangleShape(_scale);
			_button.setOrigin(_button.getGlobalBounds().width / 2, _button.getGlobalBounds().height / 2);
			_button.setPosition(_position);
			center_text();

			sf::Vector2i _mouse_pos = sf::Mouse::getPosition(window);

			bool _mouse_in_button = _button.getGlobalBounds().contains((float)_mouse_pos.x, (float)_mouse_pos.y);

			if (_mouse_in_button) {
				switch (e.type) {

				case (sf::Event::MouseMoved): {
					_button_state = state::hovered;
				}
											  break;

				case(sf::Event::MouseButtonPressed): {
					if (e.mouseButton.button == sf::Mouse::Left) {
						_button_state = state::clicked;
					}
				}
													 break;

				case (sf::Event::MouseButtonReleased): {
					if (e.mouseButton.button == sf::Mouse::Left) {
						_button_state = state::hovered;
					}
				}
													   break;

				default:
					break;
				}
			}
			else {
				_button_state = state::normal;
			}
		}

		sf::Vector2f get_position() { return _position; }
		sf::Vector2f get_scale() { return _scale; }
		sf::Text get_text() { return _text; }
		sf::RectangleShape get_button() { return _button; }

		int get_state() { return _button_state; }

		void draw_to(sf::RenderWindow& window) {
			window.draw(_button);
			window.draw(_text);
			//target.draw(_shadow, states);
		}

	private:

		sf::Text _text;
		sf::Vector2f _position;
		sf::Vector2f _scale;
		sf::Color _text_color;
		sf::Color _bg_color;
		sf::Color _border_color;

		sf::Uint16 _button_state;
		sf::RectangleShape _button;
		//sf::Text _shadow;
	};
}

#endif