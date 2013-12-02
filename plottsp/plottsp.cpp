#include <iostream>
#include <string>

#include <QtGui>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QtWidgets>
#endif

#include "view.h"

const std::string helpMessage =
    "Usage: plottsp [ -h | -p | -c | -l ]\n"
    "   -h     Show help message\n"
    "   -p     Expect edges as point pairs (default is point sequence)\n"
    "   -c     Draw the complete graph (careful!)\n"
    "   -l     Show edge length labels\n"
    "Input is read from standard input";

int main(int argc, char *argv[]) {
    View::Options options = View::PointSequence;

    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg.compare("-p") == 0) {
            options |= View::PointPairs;
        } else if (arg.compare("-c") == 0) {
            options |= View::CompleteGraph;
        } else if (arg.compare("-l") == 0) {
            options |= View::ShowLengths;
        } else if (arg.compare("-h") == 0) {
            std::cout << helpMessage << std::endl;
            return 0;
        } else {
            std::cerr << "Unknown option: " << arg << std::endl;
            std::cerr << helpMessage << std::endl;
            return 1;
        }
    }

    QApplication app(argc, argv);

    QGraphicsScene scene;
    View view(std::cin, options, &scene);
    view.show();

    return app.exec();
}
