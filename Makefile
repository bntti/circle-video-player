TARGET := circle-player
SRC := video-player.cpp
CXXFLAGS := -I /usr/include/opencv4/ -lsfml-graphics -lsfml-system -lsfml-window -lopencv_core -lopencv_imgproc -lopencv_videoio -lopencv_imgcodecs -O3 -march=native -Wshadow -Wextra -Wall

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $? -o $@

clean:
	-rm -rf $(TARGET)

.PHONY: clean
