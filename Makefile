run:
	clang++ -w -std=c++17 -o plot_shapes task.cpp $(pkg-config --cflags python3) $(pkg-config --libs python-3.12) -I/opt/homebrew/Cellar/boost/1.85.0/include -L/opt/homebrew/Cellar/boost/1.85.0/lib -lboost_system -lboost_filesystem -I/opt/homebrew/lib/python3.12/site-packages/numpy/core/include -I/opt/homebrew/Cellar/nlohmann-json/3.11.3/include

