#include "gridrender.h"

using namespace Utilities;

GridRender::GridRender(WContainerWidget *parent):WGLWidget(parent)
{
}

/// for using a texture, replase vertexCol and vColor
std::string vertexShaderSrc =
    "attribute  highp   vec3    vrtPos;                                 \n"
    "attribute  highp   vec4    vrtClr;                                 \n"
    "uniform    highp   mat4    mMatrix;                                \n"
    "uniform    highp   mat4    vMatrix;                                \n"
    "uniform    highp   mat4    cMatrix;                                \n"
    "uniform    highp   mat4    pMatrix;                                \n"
    "varying    highp   vec4    vColor;                                 \n"
    "void main(void)                                                    \n"
    "{                                                                  \n"
    "    gl_Position =                                                  \n"
    "       pMatrix * cMatrix * vMatrix * mMatrix * vec4(vrtPos, 1.0);  \n"
    "    vColor = vrtClr;                                               \n"
    "}                                                                  \n";

std::string fragmentShaderSrc =
    "varying    highp   vec4    vColor;                                 \n"
    "void main(void)                                                    \n"
    "{                                                                  \n"
    "    gl_FragColor = vColor;                                         \n"
    "}                                                                  \n";

/// \todo it should be external input, use only for testing
double objectPosition[] = {
   0.0,  5.0,  -5.0,
  -5.0, -5.0,  0.0,
   5.0, -5.0,  5.0
};
double objectColor[] = {
   1.0,  0.0,  0.0,  1.0,
   0.0,  1.0,  0.0,  1.0,
   0.0,  0.0,  1.0,  1.0
};

void GridRender::initializeGL()
{
    // Loading of shaders
    Shader vertexShader = createShader(VERTEX_SHADER);
    shaderSource(vertexShader, vertexShaderSrc);
    compileShader(vertexShader);

    Shader fragmentShader = createShader(FRAGMENT_SHADER);
    shaderSource(fragmentShader, fragmentShaderSrc);
    compileShader(fragmentShader);

    _shaderProgram = createProgram();
    attachShader(_shaderProgram, vertexShader);
    attachShader(_shaderProgram, fragmentShader);
    linkProgram(_shaderProgram);
    useProgram(_shaderProgram);

    _vertexPositionAttribute = getAttribLocation(_shaderProgram, "vrtPos");
    enableVertexAttribArray(_vertexPositionAttribute);
    _vertexColorAttribute = getAttribLocation(_shaderProgram, "vrtClr");
    enableVertexAttribArray(_vertexColorAttribute);
    _uniformModelMatrix = getUniformLocation(_shaderProgram, "mMatrix");
    _uniformViewMatrix = getUniformLocation(_shaderProgram, "vMatrix");
    _uniformClientMatrix = getUniformLocation(_shaderProgram, "cMatrix");
    _uniformProjectionMatrix = getUniformLocation(_shaderProgram, "pMatrix");

    // Preloading buffers
    _renderingObjectVertexPositionBuffer = createBuffer();
    bindBuffer(ARRAY_BUFFER, _renderingObjectVertexPositionBuffer);
    /// \todo object should be as external input, use it only for tests
    bufferDatafv<double*>(ARRAY_BUFFER, objectPosition, objectPosition + 9, STATIC_DRAW);

    _renderingObjectVertexColorBuffer = createBuffer();
    bindBuffer(ARRAY_BUFFER, _renderingObjectVertexColorBuffer);
    bufferDatafv<double*>(ARRAY_BUFFER, objectColor, objectColor + 12, STATIC_DRAW);

    /// \todo element bufer is not needed for now
    //_renderingObjectElementBuffer = createBuffer();
    //bindBuffer(ELEMENT_ARRAY_BUFFER, _renderingObjectElementBuffer);
    //bufferDataiv(ELEMENT_ARRAY_BUFFER, objectElements, count, STATIC_DRAW, DT_UNSIGNED_BYTE);

    disable(DEPTH_TEST);
    disable(CULL_FACE);
    depthFunc(LEQUAL);

    //viewport(0, 0, (unsigned) this->width().value(), (unsigned) this->height().value());
    viewport(0, 0, 640, 360);
    // Setting of matrixes
    _worldView.lookAt(
        0, 0, 10,  // camera default position
        0, 0, 0,    // camera looks at
        0, 1, 0);   // up vector
    // After that, the camera should be at(0,0,0), looks at negative Z,
    // and the sky should be at positeve Y
    _jsMatrix = createJavaScriptMatrix4();
    setJavaScriptMatrix4(_jsMatrix, _worldView);
    setClientSideLookAtHandler(
        _jsMatrix,
        0, 0, 0,
        0, 1, 0,
        0.005, 0.005);
    //_worldProjection.perspective(60, this->width().value()/this->height().value(), 0, 1000);
    _worldProjection.perspective(60, 640.0/360.0, 1, 1000);
}

void GridRender::paintGL()
{
    // Render

    // Pre-setting
    clearColor(0.5, 0.5, 0.5, 1);
    clearDepth(1);          //max
    clear(COLOR_BUFFER_BIT | DEPTH_BUFFER_BIT | STENCIL_BUFFER_BIT);

    // Apply shaders
    useProgram(_shaderProgram);

    // Draw object
    /// \todo put external rendering object here
    bindBuffer(ARRAY_BUFFER, _renderingObjectVertexPositionBuffer);
    vertexAttribPointer(_vertexPositionAttribute, 3, FLOAT, false, 0, 0);
    bindBuffer(ARRAY_BUFFER, _renderingObjectVertexColorBuffer);
    vertexAttribPointer(_vertexColorAttribute, 4, FLOAT, false, 0, 0);
    WMatrix4x4 modelMatrix;
    modelMatrix.setToIdentity();
    //modelMatrix.rotate(45, 0, 1, 0);
    //modelMatrix.translate(0, 0, -2);
    // Apply matrixes
    uniformMatrix4(_uniformModelMatrix, modelMatrix);
    uniformMatrix4(_uniformViewMatrix, _worldView);
    uniformMatrix4(_uniformClientMatrix, _jsMatrix);
    uniformMatrix4(_uniformProjectionMatrix, _worldProjection);
    drawArrays(TRIANGLES, 0, 3);
}
void GridRender::resizeGL(int width, int height)
{
    viewport(0, 0, width, height);
    //_worldProjection.perspective(60, (double)width / height, 1, 1000);
}
