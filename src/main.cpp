#include "MainWindow.h"
#include "ffmpeg/FFmpegEngine.h"
#include <QApplication>
#include <QStyleFactory>
#include <QDir>
#include <QCommandLineParser>
#include <QSurfaceFormat>

using namespace cinemastudio;

int main(int argc, char* argv[]) {
    // High DPI support
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QApplication app(argc, argv);
    app.setApplicationName("CinemaStudio");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("CinemaStudio");
    app.setStyle(QStyleFactory::create("Fusion"));

    // Set up OpenGL format for video rendering
    QSurfaceFormat format;
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setVersion(3, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);

    // Initialize FFmpeg
    FFmpegEngine::initialize();

    // Command line parser
    QCommandLineParser parser;
    parser.setApplicationDescription("CinemaStudio - Modern Video Editor");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("file", "Project file to open", "[file]");
    parser.process(app);

    // Create and show main window
    MainWindow mainWindow;
    mainWindow.show();

    // Open project file if provided
    if (!parser.positionalArguments().isEmpty()) {
        QString filePath = parser.positionalArguments().first();
        if (QFile::exists(filePath)) {
            // Will be handled by MainWindow once it's shown
        }
    }

    int result = app.exec();

    // Cleanup FFmpeg
    FFmpegEngine::cleanup();

    return result;
}
