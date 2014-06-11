#include "ANGLETest.h"

#include <vector>

class FeatureLevel9Test : public ANGLETest
{
protected:
    FeatureLevel9Test()
    {
        setWindowWidth(128);
        setWindowHeight(128);
        setConfigRedBits(8);
        setConfigGreenBits(8);
        setConfigBlueBits(8);
        setConfigAlphaBits(8);
        setClientVersion(3);
    }

    virtual void SetUp()
    {
        // Ensure that ANGLE is initialized with a Feature Level restricted
        // environment.  This is done by destroying any currently created
        // Windows/setup and recreating one.
        ANGLETest::DestroyTestWindow();
        if (!ANGLETest::InitTestWindow(EGL_D3D11_FL9_3_ONLY_DISPLAY_ANGLE))
        {
            FAIL() << "Failed to create ANGLE limited FeatureLevel test window.";
        }

        ANGLETest::SetUp();
    }

    virtual void TearDown()
    {
        ANGLETest::TearDown();

        // Ensure that ANGLE is reinitialized with a non-Feature Level restricted
        // environment.  This is done by destroying any currently created
        // Windows/setup and recreating one without restrictions.
        ANGLETest::DestroyTestWindow();
        if (!ANGLETest::InitTestWindow())
        {
            FAIL() << "Failed to create ANGLE test window.";
        }
    }

    // Point Sprite Compilation Test Vertex Shader source
    const std::string pointSpriteCompilationTestVS = SHADER_SOURCE
    (
        void main()
        {
            gl_PointSize = 100.0;
            gl_Position = vec4(1.0, 0.0, 0.0, 1.0);
        }
    );

    // Point Sprite Compilation Test Fragment Shader source
    const std::string pointSpriteCompilationTestFS = SHADER_SOURCE
    (
        precision mediump float;
        void main()
        {
            gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
        }
    );

    // Helper that checks for D3D_FEATURE_LEVEL_9_* by looking for the a hint in the renderer string.
    bool findFL9RendererString()
    {
        std::string rendererString = std::string((char*)glGetString(GL_RENDERER));
        std::transform(rendererString.begin(), rendererString.end(), rendererString.begin(), ::tolower);
        return (rendererString.find(std::string("level_9")) != std::string::npos);
    }
};

TEST_F(FeatureLevel9Test, pointSpriteCompile)
{
    bool featureLevel9StringFound = findFL9RendererString();
    EXPECT_EQ(true, featureLevel9StringFound);

    // Compiling the shaders should succeed. It's linking them that should fail.
    GLuint program = glCreateProgram();
    ASSERT_GL_NO_ERROR();
    ASSERT_NE(program, 0U);

    GLuint vs = compileShader(GL_VERTEX_SHADER, pointSpriteCompilationTestVS);
    ASSERT_GL_NO_ERROR();
    ASSERT_NE(vs, 0U);

    GLuint fs = compileShader(GL_FRAGMENT_SHADER, pointSpriteCompilationTestFS);
    ASSERT_GL_NO_ERROR();
    ASSERT_NE(fs, 0U);

    if (vs == 0 || fs == 0)
    {
        glDeleteShader(fs);
        glDeleteShader(vs);
        glDeleteProgram(program);
    }

    glAttachShader(program, vs);
    ASSERT_GL_NO_ERROR();
    glDeleteShader(vs);

    glAttachShader(program, fs);
    ASSERT_GL_NO_ERROR();
    glDeleteShader(fs);

    // Linking should fail on D3D_FEATURE_LEVEL_9_*.
    GLint linkStatus;
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
    ASSERT_EQ(0, linkStatus);

    // If the compilation failed, then check that it failed for the expected reason.
    // Do this by checking that the error log mentions Geometry Shaders.
    GLint infoLogLength;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
    std::vector<GLchar> infoLog(infoLogLength);
    glGetProgramInfoLog(program, (GLsizei)infoLog.size(), NULL, infoLog.data());

    std::string logString = std::string(infoLog.begin(), infoLog.end());
    std::transform(logString.begin(), logString.end(), logString.begin(), ::tolower);
    bool geometryShaderFoundInLog = (logString.find(std::string("geometry shader")) != std::string::npos);
    ASSERT_EQ(true, geometryShaderFoundInLog);

    glDeleteProgram(program);
}

TEST_F(FeatureLevel9Test, transformFeedback)
{
    bool featureLevel9StringFound = findFL9RendererString();
    EXPECT_EQ(true, featureLevel9StringFound);

    glGetIntegeri_v(GL_TRANSFORM_FEEDBACK_BUFFER_START, 0, NULL);
    ASSERT_GL_ERROR(GL_INVALID_OPERATION);

    glGetIntegeri_v(GL_TRANSFORM_FEEDBACK_BUFFER_SIZE, 0, NULL);
    ASSERT_GL_ERROR(GL_INVALID_OPERATION);

    glGetIntegeri_v(GL_TRANSFORM_FEEDBACK_BUFFER_BINDING, 0, NULL);
    ASSERT_GL_ERROR(GL_INVALID_OPERATION);

    glGetInteger64i_v(GL_TRANSFORM_FEEDBACK_BUFFER_START, 0, NULL);
    ASSERT_GL_ERROR(GL_INVALID_OPERATION);

    glGetInteger64i_v(GL_TRANSFORM_FEEDBACK_BUFFER_SIZE, 0, NULL);
    ASSERT_GL_ERROR(GL_INVALID_OPERATION);

    glGetInteger64i_v(GL_TRANSFORM_FEEDBACK_BUFFER_BINDING, 0, NULL);
    ASSERT_GL_ERROR(GL_INVALID_OPERATION);

    glBeginTransformFeedback(GL_TRIANGLES);
    ASSERT_GL_ERROR(GL_INVALID_OPERATION);

    glBeginTransformFeedback(GL_LINES);
    ASSERT_GL_ERROR(GL_INVALID_OPERATION);

    glBeginTransformFeedback(GL_POINTS);
    ASSERT_GL_ERROR(GL_INVALID_OPERATION);

    glEndTransformFeedback();
    ASSERT_GL_ERROR(GL_INVALID_OPERATION);

    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0);
    ASSERT_GL_ERROR(GL_INVALID_OPERATION);

    glTransformFeedbackVaryings(0, 0, NULL, GL_INTERLEAVED_ATTRIBS);
    ASSERT_GL_ERROR(GL_INVALID_OPERATION);

    glTransformFeedbackVaryings(0, 0, NULL, GL_SEPARATE_ATTRIBS);
    ASSERT_GL_ERROR(GL_INVALID_OPERATION);

    glGetTransformFeedbackVarying(0, 0, 0, NULL, NULL, NULL, NULL);
    ASSERT_GL_ERROR(GL_INVALID_OPERATION);

    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
    ASSERT_GL_ERROR(GL_INVALID_OPERATION);

    glDeleteTransformFeedbacks(0, NULL);
    ASSERT_GL_ERROR(GL_INVALID_OPERATION);

    glGenTransformFeedbacks(0, NULL);
    ASSERT_GL_ERROR(GL_INVALID_OPERATION);

    glPauseTransformFeedback();
    ASSERT_GL_ERROR(GL_INVALID_OPERATION);

    glResumeTransformFeedback();
    ASSERT_GL_ERROR(GL_INVALID_OPERATION);
}
