#include "resourceloader.h"
#include "errorchecker.h"
#include <QFile>
#include <QTextStream>

ResourceLoader::ResourceLoader()
{
}

GLuint ResourceLoader::createFullShaderProgram(
        const char *vertexFilePath,
        const char *fragmentFilePath,
        const char *geomFilePath,
        const char *tessControlFilePath,
        const char *tessEvalFilePath)
{
    // Create and compile the shaders.
    std::vector<GLuint> shaders{
        createShader(GL_VERTEX_SHADER, vertexFilePath),
        createShader(GL_FRAGMENT_SHADER, fragmentFilePath),
        createShader(GL_GEOMETRY_SHADER, geomFilePath),
        createShader(GL_TESS_CONTROL_SHADER, tessControlFilePath),
        createShader(GL_TESS_EVALUATION_SHADER, tessEvalFilePath)
    };

    return createProgramFromShaders(shaders);
}

GLuint ResourceLoader::createGeomShaderProgram(
        const char *vertexFilePath,
        const char *fragmentFilePath,
        const char *geomFilePath)
{
    // Create and compile the shaders.
    std::vector<GLuint> shaders{
        createShader(GL_VERTEX_SHADER, vertexFilePath),
        createShader(GL_FRAGMENT_SHADER, fragmentFilePath),
        createShader(GL_GEOMETRY_SHADER, geomFilePath)
    };
    return createProgramFromShaders(shaders);
}

GLuint ResourceLoader::createTessShaderProgram(
        const char *vertexFilePath,
        const char *fragmentFilePath,
        const char *tessControlFilePath,
        const char *tessEvalFilePath)
{
    // Create and compile the shaders.
    std::vector<GLuint> shaders{
        createShader(GL_VERTEX_SHADER, vertexFilePath),
        createShader(GL_FRAGMENT_SHADER, fragmentFilePath),
        createShader(GL_TESS_CONTROL_SHADER, tessControlFilePath),
        createShader(GL_TESS_EVALUATION_SHADER, tessEvalFilePath)
    };
    return createProgramFromShaders(shaders);
}

GLuint ResourceLoader::createBasicShaderProgram(const char *vertexFilePath,const char *fragmentFilePath)
{
    // Create and compile the shaders.
    std::vector<GLuint> shaders{
        createShader(GL_VERTEX_SHADER, vertexFilePath),
        createShader(GL_FRAGMENT_SHADER, fragmentFilePath),
    };
    return createProgramFromShaders(shaders);
}

GLuint ResourceLoader::createProgramFromShaders(std::vector<GLuint> &shaders)
{
    GLuint programId = glCreateProgram();

    // Attach shaders.
    for (std::vector<GLuint>::iterator iter = shaders.begin(); iter != shaders.end(); iter++) {
        GLuint shaderID = (*iter);
        glAttachShader(programId, shaderID);
    }

    // Link program.
    glLinkProgram(programId);

    // Print the info log.
    GLint result;
    GLint infoLogLength;
    glGetProgramiv(programId, GL_LINK_STATUS, &result);
    glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength > 0) {
        std::vector<char> infoLog( std::max(infoLogLength, int(1)) );
        glGetProgramInfoLog(programId, infoLogLength, NULL, &infoLog[0]);
        fprintf(stdout, "%s\n", &infoLog[0]);

        // Exit if shader program not linked.
        if (result == GL_FALSE) exit(1);
    }

    // Delete shaders.
    for (std::vector<GLuint>::iterator iter = shaders.begin(); iter != shaders.end(); iter++) {
        GLuint shaderID = (*iter);
        glDeleteShader(shaderID);
    }

    return programId;
}

GLuint ResourceLoader::createShader(GLenum shaderType, const char *filepath)
{
    GLuint shaderID = glCreateShader(shaderType);

    // Read shader file.
    std::string code;
    QString filepathStr = QString(filepath);
    QFile file(filepathStr);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        code = stream.readAll().toStdString();
    }

    // Compile shader code.
    printf("Compiling shader: %s\n", filepath);
    const char *codePtr = code.c_str();
    glShaderSource(shaderID, 1, &codePtr, NULL);
    glCompileShader(shaderID);

    // Print info log.
    GLint result;
    GLint infoLogLength;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength > 0) {
        std::vector<char> infoLog(infoLogLength);
        glGetShaderInfoLog(shaderID, infoLogLength, NULL, &infoLog[0]);
        fprintf(stdout, "%s\n", &infoLog[0]);

        // Exit if shader not compiled.
        if (result == GL_FALSE) exit(1);
    }

    return shaderID;
}

void ResourceLoader::initializeGlew()
{
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (GLEW_OK != err) fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    fprintf(stdout, "Using GLEW %s\n", glewGetString(GLEW_VERSION));

    // Check OpenGL errors but ignore GL_INVALID_ENUM, which is caused by glewExperimental.
    while ((err = glGetError()) != GL_NO_ERROR)
    {
        if (err == GL_INVALID_ENUM) continue;
        printf("OpenGL error in ResourceLoader::initializeGlew(): %d\n", err);
    }}