test_insert:
	g++ -c test_insert.cpp
	g++ test_insert.o -o sfml-app -lsfml-graphics -lsfml-window -lsfml-system
	./sfml-app

test_knn:
	g++ -c test_knn.cpp
	g++ test_knn.o -o sfml-app -lsfml-graphics -lsfml-window -lsfml-system
	./sfml-app

test_range_search:
	g++ -c test_range_search.cpp
	g++ test_range_search.o -o sfml-app -lsfml-graphics -lsfml-window -lsfml-system
	./sfml-app

test_remove:
	g++ test_remove.cpp
	./a.out