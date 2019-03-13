
#define DEPTH_STREAM_HEIGHT 480
#define DEPTH_STREAM_WIDTH 640
#define STREAM_SCALE 2
#define RIGHT_OFFSET 150
#define BOTTOM_OFFSET 0
#define FOV_H 60
#define FOV_V 49.5
#define PI 3.14159265
#define ASTRA_X 0
#define ASTRA_Y 3349.3
#define ASTRA_Z 0

#include <SFML/Graphics.hpp>
#include <astra/astra.hpp>
#include <iostream>
#include <fstream>
#include <cstring>
#include "button.cpp"
#include <Windows.h>
#include <math.h>

std::string workingdir(bool print = false)
{
	char buf[256];
	GetCurrentDirectoryA(256, buf);
	if (print)
		std::cout << buf << std::endl;
	return std::string(buf) + '\\';
}

char* get_joint_name(astra::Joint joint) {

	char* joint_name;
	switch (joint.type()) {
	case astra::JointType::Head:
		joint_name = "Head";
		break;
	case astra::JointType::Neck:
		joint_name = "Neck";
		break;
	case astra::JointType::ShoulderSpine:
		joint_name = "Spine Top";
		break;
	case astra::JointType::LeftShoulder:
		joint_name = "Left Shoulder";
		break;
	case astra::JointType::LeftElbow:
		joint_name = "Left Elbow";
		break;
	case astra::JointType::LeftWrist:
		joint_name = "Left Wrist";
		break;
	case astra::JointType::LeftHand:
		joint_name = "Left Hand";
		break;
	case astra::JointType::RightShoulder:
		joint_name = "Right Shoulder";
		break;
	case astra::JointType::RightElbow:
		joint_name = "Right Elbow";
		break;
	case astra::JointType::RightWrist:
		joint_name = "Right Wrist";
		break;
	case astra::JointType::RightHand:
		joint_name = "Right Hand";
		break;
	case astra::JointType::MidSpine:
		joint_name = "Spine Middle";
		break;
	case astra::JointType::BaseSpine:
		joint_name = "Spine Base";
		break;
	case astra::JointType::LeftHip:
		joint_name = "Left Hip";
		break;
	case astra::JointType::LeftKnee:
		joint_name = "Left Knee";
		break;
	case astra::JointType::LeftFoot:
		joint_name = "Left Foot";
		break;
	case astra::JointType::RightHip:
		joint_name = "Right Hip";
		break;
	case astra::JointType::RightKnee:
		joint_name = "Right Knee";
		break;
	case astra::JointType::RightFoot:
		joint_name = "Right Foot";
		break;
	default:
		joint_name = "Unknown Joint";
		break;
	}
	return joint_name;
}

class sfLine : public sf::Drawable
{
public:
	sfLine(const sf::Vector2f& point1, const sf::Vector2f& point2, sf::Color color, float thickness)
		: color_(color)
	{
		const sf::Vector2f direction = point2 - point1;
		const sf::Vector2f unitDirection = direction / std::sqrt(direction.x*direction.x + direction.y*direction.y);
		const sf::Vector2f normal(-unitDirection.y, unitDirection.x);

		const sf::Vector2f offset = (thickness / 2.f) * normal;

		vertices_[0].position = point1 + offset;
		vertices_[1].position = point2 + offset;
		vertices_[2].position = point2 - offset;
		vertices_[3].position = point1 - offset;

		for (int i = 0; i<4; ++i)
			vertices_[i].color = color;
	}

	void draw(sf::RenderTarget &target, sf::RenderStates states) const
	{
		target.draw(vertices_, 4, sf::Quads, states);
	}

private:
	sf::Vertex vertices_[4];
	sf::Color color_;
};

class BodyVisualizer : public astra::FrameListener
{
public:
	static sf::Color get_body_color(std::uint8_t bodyId)
	{
		if (bodyId == 0)
		{
			// Handle no body separately - transparent
			return sf::Color(0x00, 0x00, 0x00, 0x00);
		}
		// Case 0 below could mean bodyId == 25 or
		// above due to the "% 24".
		switch (bodyId % 24) {
		case 0:
			return sf::Color(0x00, 0x88, 0x00, 0xFF);
		case 1:
			return sf::Color(0x00, 0x00, 0xFF, 0xFF);
		case 2:
			return sf::Color(0x88, 0x00, 0x00, 0xFF);
		case 3:
			return sf::Color(0x00, 0xFF, 0x00, 0xFF);
		case 4:
			return sf::Color(0x00, 0x00, 0x88, 0xFF);
		case 5:
			return sf::Color(0xFF, 0x00, 0x00, 0xFF);

		case 6:
			return sf::Color(0xFF, 0x88, 0x00, 0xFF);
		case 7:
			return sf::Color(0xFF, 0x00, 0xFF, 0xFF);
		case 8:
			return sf::Color(0x88, 0x00, 0xFF, 0xFF);
		case 9:
			return sf::Color(0x00, 0xFF, 0xFF, 0xFF);
		case 10:
			return sf::Color(0x00, 0xFF, 0x88, 0xFF);
		case 11:
			return sf::Color(0xFF, 0xFF, 0x00, 0xFF);

		case 12:
			return sf::Color(0x00, 0x88, 0x88, 0xFF);
		case 13:
			return sf::Color(0x00, 0x88, 0xFF, 0xFF);
		case 14:
			return sf::Color(0x88, 0x88, 0x00, 0xFF);
		case 15:
			return sf::Color(0x88, 0xFF, 0x00, 0xFF);
		case 16:
			return sf::Color(0x88, 0x00, 0x88, 0xFF);
		case 17:
			return sf::Color(0xFF, 0x00, 0x88, 0xFF);

		case 18:
			return sf::Color(0xFF, 0x88, 0x88, 0xFF);
		case 19:
			return sf::Color(0xFF, 0x88, 0xFF, 0xFF);
		case 20:
			return sf::Color(0x88, 0x88, 0xFF, 0xFF);
		case 21:
			return sf::Color(0x88, 0xFF, 0xFF, 0xFF);
		case 22:
			return sf::Color(0x88, 0xFF, 0x88, 0xFF);
		case 23:
			return sf::Color(0xFF, 0xFF, 0x88, 0xFF);
		default:
			return sf::Color(0xAA, 0xAA, 0xAA, 0xFF);
		}
	}

	void init_depth_texture(int width, int height)
	{
		if (displayBuffer_ == nullptr || width != depthWidth_ || height != depthHeight_)
		{
			depthWidth_ = width;
			depthHeight_ = height;
			int byteLength = depthWidth_ * depthHeight_ * 4;

			displayBuffer_ = BufferPtr(new uint8_t[byteLength]);
			std::memset(displayBuffer_.get(), 0, byteLength);

			texture_.create(depthWidth_, depthHeight_);
			sprite_.setTexture(texture_, true);
			sprite_.setPosition(0, 0);
		}
	}

	void init_overlay_texture(int width, int height)
	{
		if (overlayBuffer_ == nullptr || width != overlayWidth_ || height != overlayHeight_)
		{
			overlayWidth_ = width;
			overlayHeight_ = height;
			int byteLength = overlayWidth_ * overlayHeight_ * 4;

			overlayBuffer_ = BufferPtr(new uint8_t[byteLength]);
			std::fill(&overlayBuffer_[0], &overlayBuffer_[0] + byteLength, 0);

			overlayTexture_.create(overlayWidth_, overlayHeight_);
			overlaySprite_.setTexture(overlayTexture_, true);
			overlaySprite_.setPosition(0, 0);
		}
	}

	void check_fps()
	{
		double fpsFactor = 0.02;

		std::clock_t newTimepoint = std::clock();
		long double frameDuration = (newTimepoint - lastTimepoint_) / static_cast<long double>(CLOCKS_PER_SEC);

		frameDuration_ = frameDuration * fpsFactor + frameDuration_ * (1 - fpsFactor);
		lastTimepoint_ = newTimepoint;
		double fps = 1.0 / frameDuration_;

		printf("FPS: %3.1f (%3.4Lf ms)\n", fps, frameDuration_ * 1000);
	}

	void processDepth(astra::Frame& frame)
	{
		const astra::DepthFrame depthFrame = frame.get<astra::DepthFrame>();

		if (!depthFrame.is_valid()) { return; }

		int width = depthFrame.width();
		int height = depthFrame.height();

		init_depth_texture(width, height);

		const int16_t* depthPtr = depthFrame.data();
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				int index = (x + y * width);
				int index4 = index * 4;

				int16_t depth = depthPtr[index];
				uint8_t value = depth % 255;

				displayBuffer_[index4] = value;
				displayBuffer_[index4 + 1] = value;
				displayBuffer_[index4 + 2] = value;
				displayBuffer_[index4 + 3] = 255;
			}
		}

		texture_.update(displayBuffer_.get());
	}

	void processBodies(astra::Frame& frame)
	{
		astra::BodyFrame bodyFrame = frame.get<astra::BodyFrame>();

		jointPositions_.clear();
		circles_.clear();
		circleShadows_.clear();
		boneLines_.clear();
		boneShadows_.clear();

		if (!bodyFrame.is_valid() || bodyFrame.info().width() == 0 || bodyFrame.info().height() == 0)
		{
			clear_overlay();
			return;
		}

		const float jointScale = bodyFrame.info().width() / 120.f;

		const auto& bodies = bodyFrame.bodies();

		for (auto& body : bodies)
		{
			//printf("Processing frame #%d body %d left hand: %u\n",
			//bodyFrame.frame_index(), body.id(), unsigned(body.hand_poses().left_hand()));
			for (auto& joint : body.joints())
			{
				jointPositions_.push_back(joint.depth_position());
				joint.type();
			}

			update_body(body, jointScale);
		}

		const auto& floor = bodyFrame.floor_info(); //floor
		if (floor.floor_detected())
		{
			const auto& p = floor.floor_plane();
			std::cout << "Floor plane: ["
				<< p.a() << ", " << p.b() << ", " << p.c() << ", " << p.d()
				<< "]" << std::endl;

		}

		const auto& bodyMask = bodyFrame.body_mask();
		const auto& floorMask = floor.floor_mask();

		update_overlay(bodyMask, floorMask);
	}

	void update_body(astra::Body body,
		const float jointScale)
	{
		const auto& joints = body.joints();

		if (joints.empty())
		{
			return;
		}

		for (const auto& joint : joints)
		{
			astra::JointType type = joint.type();
			const auto& pos = joint.depth_position();

			if (joint.status() == astra::JointStatus::NotTracked)
			{
				continue;
			}

			auto radius = jointRadius_ * jointScale; // pixels
			sf::Color circleShadowColor(0, 0, 0, 255);

			auto color = sf::Color(0x00, 0xFF, 0x00, 0xFF);

			if ((type == astra::JointType::LeftHand && astra::HandPose::Grip == body.hand_poses().left_hand()) ||
				(type == astra::JointType::RightHand &&  astra::HandPose::Grip == body.hand_poses().right_hand()))
			{
				radius *= 1.5f;
				circleShadowColor = sf::Color(255, 255, 255, 255);
				color = sf::Color(0x00, 0xAA, 0xFF, 0xFF);
			}

			const auto shadowRadius = radius + shadowRadius_ * jointScale;
			const auto radiusDelta = shadowRadius - radius;

			sf::CircleShape circle(radius);

			circle.setFillColor(sf::Color(color.r, color.g, color.b, 255));
			circle.setPosition(pos.x - radius, pos.y - radius);
			circles_.push_back(circle);

			sf::CircleShape shadow(shadowRadius);
			shadow.setFillColor(circleShadowColor);
			shadow.setPosition(circle.getPosition() - sf::Vector2f(radiusDelta, radiusDelta));
			circleShadows_.push_back(shadow);
		}

		update_bone(joints, jointScale, astra::JointType::Head, astra::JointType::Neck);
		update_bone(joints, jointScale, astra::JointType::Neck, astra::JointType::ShoulderSpine);

		update_bone(joints, jointScale, astra::JointType::ShoulderSpine, astra::JointType::LeftShoulder);
		update_bone(joints, jointScale, astra::JointType::LeftShoulder, astra::JointType::LeftElbow);
		update_bone(joints, jointScale, astra::JointType::LeftElbow, astra::JointType::LeftWrist);
		update_bone(joints, jointScale, astra::JointType::LeftWrist, astra::JointType::LeftHand);

		update_bone(joints, jointScale, astra::JointType::ShoulderSpine, astra::JointType::RightShoulder);
		update_bone(joints, jointScale, astra::JointType::RightShoulder, astra::JointType::RightElbow);
		update_bone(joints, jointScale, astra::JointType::RightElbow, astra::JointType::RightWrist);
		update_bone(joints, jointScale, astra::JointType::RightWrist, astra::JointType::RightHand);

		update_bone(joints, jointScale, astra::JointType::ShoulderSpine, astra::JointType::MidSpine);
		update_bone(joints, jointScale, astra::JointType::MidSpine, astra::JointType::BaseSpine);

		update_bone(joints, jointScale, astra::JointType::BaseSpine, astra::JointType::LeftHip);
		update_bone(joints, jointScale, astra::JointType::LeftHip, astra::JointType::LeftKnee);
		update_bone(joints, jointScale, astra::JointType::LeftKnee, astra::JointType::LeftFoot);

		update_bone(joints, jointScale, astra::JointType::BaseSpine, astra::JointType::RightHip);
		update_bone(joints, jointScale, astra::JointType::RightHip, astra::JointType::RightKnee);
		update_bone(joints, jointScale, astra::JointType::RightKnee, astra::JointType::RightFoot);
	}

	void update_bone(const astra::JointList& joints,
		const float jointScale, astra::JointType j1,
		astra::JointType j2)
	{
		const auto& joint1 = joints[int(j1)];
		const auto& joint2 = joints[int(j2)];

		if (joint1.status() == astra::JointStatus::NotTracked ||
			joint2.status() == astra::JointStatus::NotTracked)
		{
			//don't render bones between untracked joints
			return;
		}

		//actually depth position, not world position
		const auto& jp1 = joint1.depth_position();
		const auto& jp2 = joint2.depth_position();

		auto p1 = sf::Vector2f(jp1.x, jp1.y);
		auto p2 = sf::Vector2f(jp2.x, jp2.y);

		sf::Color color(255, 255, 255, 255);
		float thickness = lineThickness_ * jointScale;
		if (joint1.status() == astra::JointStatus::LowConfidence ||
			joint2.status() == astra::JointStatus::LowConfidence)
		{
			color = sf::Color(128, 128, 128, 255);
			thickness *= 0.5f;
		}

		boneLines_.push_back(sfLine(p1,
			p2,
			color,
			thickness));
		const float shadowLineThickness = thickness + shadowRadius_ * jointScale * 2.f;
		boneShadows_.push_back(sfLine(p1,
			p2,
			sf::Color(0, 0, 0, 255),
			shadowLineThickness));
	}

	void update_overlay(const astra::BodyMask& bodyMask,
		const astra::FloorMask& floorMask)
	{
		const auto* bodyData = bodyMask.data();
		const auto* floorData = floorMask.data();
		const int width = bodyMask.width();
		const int height = bodyMask.height();

		init_overlay_texture(width, height);

		const int length = width * height;

		for (int i = 0; i < length; i++)
		{
			const auto bodyId = bodyData[i];
			const auto isFloor = floorData[i];

			sf::Color color(0x0, 0x0, 0x0, 0x0);

			if (bodyId != 0)
			{
				color = get_body_color(bodyId);
			}
			else if (isFloor != 0)
			{
				color = sf::Color(0x0, 0x0, 0xFF, 0x88);
			}

			const int rgbaOffset = i * 4;
			overlayBuffer_[rgbaOffset] = color.r;
			overlayBuffer_[rgbaOffset + 1] = color.g;
			overlayBuffer_[rgbaOffset + 2] = color.b;
			overlayBuffer_[rgbaOffset + 3] = color.a;
		}

		overlayTexture_.update(overlayBuffer_.get());
	}

	void clear_overlay()
	{
		int byteLength = overlayWidth_ * overlayHeight_ * 4;
		std::fill(&overlayBuffer_[0], &overlayBuffer_[0] + byteLength, 0);

		overlayTexture_.update(overlayBuffer_.get());
	}

	void log_data(astra::StreamReader& reader, astra::Frame& frame) {

		frameNumber_++;
		std::ofstream file;
		std::ofstream raw;
		std::string file_dir = output_dir_ + patient_name_;
		CreateDirectory(file_dir.c_str(), NULL);
		file.open(file_dir + "\\astra_out.txt", std::ios::out | std::ios::app);
		raw.open(file_dir + "\\raw_data.txt", std::ios::out | std::ios::app);
		std::cout << file_dir << std::endl;

		// Get body data
		astra::BodyFrame bodyFrame = frame.get<astra::BodyFrame>();
		const auto& bodies = bodyFrame.bodies();

		// Get depth data
		astra::DepthFrame depthFrame = frame.get<astra::DepthFrame>();
		const auto depthData = depthFrame.data();
		const int frameWidth = depthFrame.width();

		// Output position (pixel_x, pixel_y, depth) of each joint
		double x, y, z;
		double deg_shoulders;
		double x_L = 10000, y_L, z_L;
		double x_R = 10000, y_R, z_R;
		for (auto& body : bodies)
		{
			if (body.joints_enabled()) {
				file << "Frame number: " << frameNumber_ << std::endl;
				file << "Body Id: " << std::to_string(body.id()) << std::endl;
				for (auto& joint : body.joints()) {
					x = joint.world_position().x;
					y = joint.world_position().y;
					z = joint.world_position().z;

					if (!(x == ASTRA_X && y == ASTRA_Y && z == ASTRA_Z)) {
						file << get_joint_name(joint) << " position: (" << x << ", " << y << ", " << z << ")" << std::endl;
						raw << get_joint_name(joint) << "," << x << "," << y << "," << z << ";";

						if (get_joint_name(joint) == "Left Shoulder") {
							x_L = x;
							y_L = y;
							z_L = z;
						}
						if (get_joint_name(joint) == "Right Shoulder") {
							x_R = x;
							y_R = y;
							z_R = z;
						}
					}
				}
				//deg_shoulders = asin()
				if (x_L != 10000 && x_R != 10000) {
					deg_shoulders = asin((y_R - y_L) / (sqrt(pow((x_R - x_L), 2) + pow((y_R - y_L), 2) + pow((z_R - z_L), 2)))) * 180 / PI;
					file << "Scoliosis Shoulder Angle: " << deg_shoulders << std::endl;

				}
				x_L = 10000;
				x_R = 10000;
				file << std::endl;
				raw << std::endl;
				
			}
		}

		file.close();
	}

	virtual void on_frame_ready(astra::StreamReader& reader,
		astra::Frame& frame) override
	{
		if (!paused_) {
			processDepth(frame);
			processBodies(frame);
			log_data(reader, frame);
			check_fps();
		}
	}

	void draw_bodies(sf::RenderWindow& window)
	{
		const float scaleX = (window.getView().getSize().x - offset_x_) / overlayWidth_;
		const float scaleY = (window.getView().getSize().y - offset_y_) / overlayHeight_;

		sf::RenderStates states;
		sf::Transform transform;
		transform.scale(scaleX, scaleY);
		states.transform *= transform;

		for (const auto& bone : boneShadows_)
			window.draw(bone, states);

		for (const auto& c : circleShadows_)
			window.draw(c, states);

		for (const auto& bone : boneLines_)
			window.draw(bone, states);

		for (auto& c : circles_)
			window.draw(c, states);

	}

	void draw_to(sf::RenderWindow& window)
	{
		if (displayBuffer_ != nullptr)
		{
			const float scaleX = (window.getView().getSize().x - offset_x_) / depthWidth_;
			const float scaleY = (window.getView().getSize().y - offset_y_) / depthHeight_;
			sprite_.setScale(scaleX, scaleY);

			window.draw(sprite_); // depth
		}

		if (overlayBuffer_ != nullptr)
		{
			const float scaleX = (window.getView().getSize().x - offset_x_) / overlayWidth_;
			const float scaleY = (window.getView().getSize().y - offset_y_) / overlayHeight_;
			overlaySprite_.setScale(scaleX, scaleY);
			window.draw(overlaySprite_); //bodymask and floormask
		}

		draw_bodies(window);
	}

	void set_offsets(int x, int y) {
		offset_x_ = x;
		offset_y_ = y;
	}

	void toggle_pause(gui::button& record_button) {
		if (paused_) {
			record_button.set_text("Stop Recording");
			std::cout << "System Unpaused!" << std::endl;
		}
		else {
			record_button.set_text("Start Recording");
			std::cout << "System Paused!" << std::endl;
		}
		paused_ = !paused_;
	}

	void set_patient_name(std::string name) {
		patient_name_ = name;
	}

	void set_output_dir(std::string dir) {
		output_dir_ = dir;
	}

private:
	long double frameDuration_{ 0 };
	std::clock_t lastTimepoint_{ 0 };
	sf::Texture texture_;
	sf::Sprite sprite_;

	using BufferPtr = std::unique_ptr < uint8_t[] >;
	BufferPtr displayBuffer_{ nullptr };

	std::vector<astra::Vector2f> jointPositions_;

	int depthWidth_{ 0 };
	int depthHeight_{ 0 };
	int overlayWidth_{ 0 };
	int overlayHeight_{ 0 };

	std::vector<sfLine> boneLines_;
	std::vector<sfLine> boneShadows_;
	std::vector<sf::CircleShape> circles_;
	std::vector<sf::CircleShape> circleShadows_;

	float lineThickness_{ 0.5f }; // pixels
	float jointRadius_{ 1.0f };   // pixels
	float shadowRadius_{ 0.5f };  // pixels

	BufferPtr overlayBuffer_{ nullptr };
	sf::Texture overlayTexture_;
	sf::Sprite overlaySprite_;

	int frameNumber_ = 0;
	bool paused_ = true;
	int offset_x_{ 0 };
	int offset_y_{ 0 };
	std::string patient_name_ = "Unknown";
	std::string output_dir_ = workingdir();
};

astra::DepthStream configure_depth(astra::StreamReader& reader)
{
	auto depthStream = reader.stream<astra::DepthStream>();

	//We don't have to set the mode to start the stream, but if you want to here is how:
	astra::ImageStreamMode depthMode;

	depthMode.set_width(640);
	depthMode.set_height(480);
	depthMode.set_pixel_format(astra_pixel_formats::ASTRA_PIXEL_FORMAT_DEPTH_MM);
	depthMode.set_fps(30);

	depthStream.set_mode(depthMode);

	return depthStream;
}

int main(int argc, const char** argv) {

	astra::initialize();

	sf::RenderWindow window(
		sf::VideoMode(
			DEPTH_STREAM_WIDTH * STREAM_SCALE, 
			DEPTH_STREAM_HEIGHT * STREAM_SCALE), 
		"Astra body tracker");

	const char* licenseString = "<INSERT LICENSE KEY HERE>";
	orbbec_body_tracking_set_license(licenseString);

#ifdef _WIN32
	auto fullscreenStyle = sf::Style::None;
#else
	auto fullscreenStyle = sf::Style::Fullscreen;
#endif

	BodyVisualizer listener;
	if (argc >= 3) {
		listener.set_patient_name(argv[1]);
		listener.set_output_dir(argv[2]);
	}
	listener.set_offsets(RIGHT_OFFSET, BOTTOM_OFFSET);

	const sf::VideoMode fullScreenMode = sf::VideoMode::getFullscreenModes()[0];
	const sf::VideoMode windowedMode(DEPTH_STREAM_WIDTH * STREAM_SCALE + RIGHT_OFFSET, 
		DEPTH_STREAM_HEIGHT * STREAM_SCALE + BOTTOM_OFFSET);
	bool isFullScreen = false;

	astra::StreamSet sensor;
	astra::StreamReader reader = sensor.create_reader();

	reader.stream<astra::DepthStream>().start();
	reader.stream<astra::BodyStream>().start();

	reader.add_listener(listener);

	sf::Font font;

	if (!font.loadFromFile(workingdir() + "Roboto-Black.ttf")) {
		std::cerr << "Error loading Roboto-Black font" << std::endl;
	}

	sf::Text text("Start Recording", font, 15);
	text.setColor(sf::Color::Red);
	gui::button record_button(text, font, sf::Vector2f(window.getSize().x - 75.f, 40.f));
	record_button.set_scale(sf::Vector2f(150.f, 80.f));

	while (window.isOpen()) {

		astra_update();

		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::KeyPressed:
			{
				// TODO: turn these into buttons that can be clicked and have the same functionality.
				if (event.key.code == sf::Keyboard::C && event.key.control)
				{
					window.close();
				}
				switch (event.key.code)
				{
				case sf::Keyboard::Escape:
					window.close();
					break;
				case sf::Keyboard::F:
					if (isFullScreen)
					{
						window.create(windowedMode, "Simple Body Viewer", sf::Style::Default);
					}
					else
					{
						window.create(fullScreenMode, "Simple Body Viewer", fullscreenStyle);
					}
					isFullScreen = !isFullScreen;
					break;
				case sf::Keyboard::Space:
					listener.toggle_pause(record_button);
					break;
				default:
					break;
				}
				break;
			}
			default:
				break;
			}

			record_button.update(event, window);
			if (record_button.get_state() == gui::state::clicked) {
				listener.toggle_pause(record_button);
			}
		}

		// clear the window with black color
		window.clear(sf::Color::Black);

		listener.draw_to(window);
		record_button.draw_to(window);
		window.display();
	}

	astra::terminate();

	return 0;
}
