#include <SFML/Graphics.hpp>
#include <chrono>
#include <iostream>
#include <opencv4/opencv2/opencv.hpp>
#include <thread>

const int DEFAULT_WIDTH = 1280;
const int DEFAULT_HEIGHT = 720;

int main(int argc, char** argv) {
  double brightness = 1.5;
  int target_pixel_size = 10;
  bool full_rad = false;
  std::string filename = "";
  bool color = false;
  bool help = false;

  // Manage arguments.
  for (int i = 1; i < argc; ++i) {
    bool skip = false;
    int len = strlen(argv[i]);
    if (argv[i][0] != '-') {
      filename = argv[i];
      continue;
    }
    for (int j = 1; j < len; ++j) {
      switch (argv[i][j]) {
        case 'b':
          brightness = std::stod(argv[i + 1]);
          skip = true;
          break;
        case 'c':
          color = true;
          break;
        case 'h':
          help = true;
          break;
        case 'r':
          full_rad = true;
          break;
        case 's':
          target_pixel_size = atoi(argv[i + 1]);
          skip = true;
          break;
        default:
          std::cout << "Invalid argument: '-" << argv[i][j] << "'" << std::endl;
          exit(1);
      }
    }
    if (skip) ++i;
  }
  if (help || filename == "") {
    std::cout << "Usage: " << argv[0] << " <args> <filename>\n";
    std::cout << "\n";
    std::cout << "Flags:\n";
    std::cout << "\t'-b <brightness>' | Brightness factor (double).\n";
    std::cout << "\t'-c' | Enable color.\n";
    std::cout << "\t'-h' | Show this menu and exit.\n";
    std::cout << "\t'-r' | Use larger radius for circles allowing for them to "
                 "fully fill an area.\n";
    std::cout << "\t'-s <target_pixel_size>' | Size of one 'pixel'. Use "
                 "smaller number for more circles (integer).\n";
    exit(0);
  }

  cv::VideoCapture cap(filename);
  if (!cap.isOpened()) {
    std::cout << "Error opening video stream or file '" << filename << "'" << std::endl;
    return 1;
  }

  sf::Color background_color(0, 0, 0);
  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;
  sf::RenderWindow window(sf::VideoMode(DEFAULT_WIDTH, DEFAULT_HEIGHT), "", sf::Style::Default,
                          settings);

  int frame_count = 0;
  int fps = cap.get(cv::CAP_PROP_FPS);

  auto start_time = std::chrono::steady_clock::now();
  while (1) {
    sf::Vector2u window_size = window.getSize();
    int tmp = std::min(window_size.x / 16.0, window_size.y / 9.0);
    int im_width = std::floor(tmp / (double)target_pixel_size) * 16;
    int im_height = std::floor(tmp / (double)target_pixel_size) * 9;

    double pixel_size = 16.0 * tmp / im_width;

    double radius = pixel_size / 2.0;
    if (full_rad) radius *= std::sqrt(2);

    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
        return 0;
      } else if (event.type == sf::Event::Resized) {
        // update the view to the new size of the window
        sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
        window.setView(sf::View(visibleArea));
      }
    }
    window.clear(sf::Color::Black);
    ++frame_count;

    cv::Mat frame;
    cap >> frame;

    if (frame.empty()) break;

    cv::resize(frame, frame, cv::Size(im_width, im_height));

    for (int x = 0; x < im_width; ++x) {
      for (int y = 0; y < im_height; ++y) {
        // Convert color to grayscale manually
        double b = frame.at<cv::Vec3b>(y, x)[0];
        double g = frame.at<cv::Vec3b>(y, x)[1];
        double r = frame.at<cv::Vec3b>(y, x)[2];
        double val = brightness * (0.299 * r + 0.587 * g + 0.114 * b);
        if (color) {
          if ((x + y) % 3 == 0) val = r;
          if ((x + y) % 3 == 1) val = b;
          if ((x + y) % 3 == 2) val = g;
        }

        // Calculate radius from grayscale value
        double newradius = std::min(1.0, val / 255.0) * radius;
        if (newradius > 0) {
          sf::CircleShape circle(newradius);
          circle.setPosition(sf::Vector2f(x * pixel_size + pixel_size / 2.0 - newradius,
                                          y * pixel_size + pixel_size / 2.0 - newradius));
          if (color) {
            if ((x + y) % 3 == 0) circle.setFillColor(sf::Color(255, 0, 0));
            if ((x + y) % 3 == 1) circle.setFillColor(sf::Color(0, 255, 0));
            if ((x + y) % 3 == 2) circle.setFillColor(sf::Color(0, 0, 255));
          } else {
            circle.setFillColor(sf::Color(255, 255, 255));
          }
          window.draw(circle);
        }
      }
    }
    window.display();

    // Wait to draw next frame
    int time_now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                          std::chrono::steady_clock::now().time_since_epoch())
                          .count();
    int current_frame_time_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(start_time.time_since_epoch())
            .count() +
        frame_count * (1000 / fps);
    if (time_now_ms < current_frame_time_ms)
      std::this_thread::sleep_for(std::chrono::milliseconds(current_frame_time_ms - time_now_ms));
  }
}
