
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

#include <astra/astra.hpp>
#include <iostream>
#include <fstream>
#include <cstring>
#include <Windows.h>
#include <math.h>
#include <chrono>

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

class BodyVisualizer : public astra::FrameListener
{
public:

	void processBodies(astra::Frame& frame)
	{
		astra::BodyFrame bodyFrame = frame.get<astra::BodyFrame>();

		const auto& floor = bodyFrame.floor_info(); //floor
		if (floor.floor_detected())
		{
			const auto& p = floor.floor_plane();
			//std::cout << "Floor plane: ["
			//	<< p.a() << ", " << p.b() << ", " << p.c() << ", " << p.d()
			//	<< "]" << std::endl;
		}
	}

	void log_data(astra::StreamReader& reader, astra::Frame& frame) {

		using namespace std;

		frameNumber_++;
		current_time_ = chrono::high_resolution_clock::now();
		auto duration = chrono::duration_cast<chrono::milliseconds>(current_time_ - last_time_);
		last_time_ = current_time_;

		// Get body data
		astra::BodyFrame bodyFrame = frame.get<astra::BodyFrame>();
		const auto& bodies = bodyFrame.bodies();

		// Output position (pixel_x, pixel_y, depth) of each joint
		double x, y, z;
		double deg_shoulders;
		double x_L = 10000, y_L, z_L;
		double x_R = 10000, y_R, z_R;
		for (auto& body : bodies)
		{
			if (body.joints_enabled()) {
				cout << "{";
				cout << "\"frame_number\": " << frameNumber_ << ",";
				cout << "\"time\": " << duration.count() << ",";
				cout << "\"body_id\": " << to_string(body.id()) << ",";
				cout << "\"joints\": {";
				bool first_joint = TRUE;
				for (auto& joint : body.joints()) {
					if (first_joint)
						first_joint = FALSE;
					else
						cout << ",";
					x = joint.world_position().x;
					y = joint.world_position().y;
					z = joint.world_position().z;

					if (!(x == ASTRA_X && y == ASTRA_Y && z == ASTRA_Z)) {
						cout << "\"" << get_joint_name(joint) << "\": {";
						cout << "\"x\": " << x << ",";
						cout << "\"y\": " << y << ",";
						cout << "\"z\": " << z;
						cout << "}";

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
				cout << "}";
				if (x_L != 10000 && x_R != 10000) {
					deg_shoulders = asin((y_R - y_L) / (sqrt(pow((x_R - x_L), 2) + pow((y_R - y_L), 2) + pow((z_R - z_L), 2)))) * 180 / PI;
					if (!isnan(deg_shoulders)) 
						cout << ",\"shoulder_angle\": " << deg_shoulders;
				}
				x_L = 10000;
				x_R = 10000;
				cout << "}" << endl;
			}
		}
	}

	virtual void on_frame_ready(astra::StreamReader& reader,
		astra::Frame& frame) override
	{
		processBodies(frame);
		log_data(reader, frame);
	}

private:
	std::chrono::high_resolution_clock::time_point current_time_;
	std::chrono::high_resolution_clock::time_point last_time_ = std::chrono::high_resolution_clock::now();

	int frameNumber_ = 0;
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

	const char* licenseString = "<INSERT LICENSE KEY HERE>";
	orbbec_body_tracking_set_license(licenseString);

	BodyVisualizer listener;

	astra::StreamSet sensor;
	astra::StreamReader reader = sensor.create_reader();

	reader.stream<astra::DepthStream>().start();
	reader.stream<astra::BodyStream>().start();

	reader.add_listener(listener);

	while (TRUE) {
		astra_update();
	}

	astra::terminate();

	return 0;
}
