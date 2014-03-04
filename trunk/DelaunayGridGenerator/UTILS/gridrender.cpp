#include "gridrender.h"

using namespace Utilities;

GridRender::GridRender(WContainerWidget *parent):
    WGLWidget(parent),
    _renderingMode(MODE_3D),
    _PlcNodesColor(1.0, 1.0, 1.0, 1.0),
    _PlcSegmentsColor(0.0, 1.0, 0.0, 1.0)
{
    /// \todo Just TEST
    DelaunayGridGenerator::CommonPlc3D *_myCommonPlc3D
            = new DelaunayGridGenerator::CommonPlc3D();

    _myCommonPlc3D->createNode(FEM::Node3D(0.0,0.0,0.0));
    _myCommonPlc3D->createNode(FEM::Node3D(1.0,0.0,0.0));
    _myCommonPlc3D->createNode(FEM::Node3D(0.0,1.0,0.0));
    _myCommonPlc3D->createNode(FEM::Node3D(1.0,1.0,0.0));
    _myCommonPlc3D->createNode(FEM::Node3D(0.0,0.0,1.0));
    _myCommonPlc3D->createNode(FEM::Node3D(1.0,0.0,1.0));
    _myCommonPlc3D->createNode(FEM::Node3D(0.0,1.0,1.0));
    _myCommonPlc3D->createNode(FEM::Node3D(1.0,1.0,1.0));

    _myCommonPlc3D->updateMaxAndMinCoordinates();

    _myCommonPlc3D->createSegment(0,1);
    _myCommonPlc3D->createSegment(0,2);
    _myCommonPlc3D->createSegment(1,3);
    _myCommonPlc3D->createSegment(2,3);
    _myCommonPlc3D->createSegment(4,5);
    _myCommonPlc3D->createSegment(4,6);
    _myCommonPlc3D->createSegment(5,7);
    _myCommonPlc3D->createSegment(6,7);

    {int _n[]={0,1,2}; _myCommonPlc3D->createFacet(_n);}
    {int _n[]={5,6,7}; _myCommonPlc3D->createFacet(_n);}

    setRenderingPiecewiseLinearComplex(_myCommonPlc3D);
    //setRenderingPlcNodesColor(1,0,0,1);
}

/// for using a texture, replase vertexCol and vColor
std::string vertexShaderSrc =
    "attribute  highp   vec3    vrtPos;                                 \n"
    "//attribute  highp   vec4    vrtClr;                                 \n"
    "uniform    highp   vec4    vrtClr;                                 \n"
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
    "precision  mediump float;                                          \n"
    "varying    highp   vec4    vColor;                                 \n"
    "void main(void)                                                    \n"
    "{                                                                  \n"
    "    gl_FragColor = vColor;                                         \n"
    "    //gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);                       \n"
    "}                                                                  \n";

void GridRender::setRenderingPiecewiseLinearComplex(
        DelaunayGridGenerator::CommonPlc2D *renderingPlc2D) throw (std::logic_error)
{
    _refToRenderingPlc2D = renderingPlc2D;
    setRenderingMode(MODE_2D);
}

void GridRender::_initializePlc2DNodesBuffers()
{

}

void GridRender::_initializePlc2DFacetsBuffers()
{

}

void GridRender::_drawPlc2DNodes()
{

}

void GridRender::_drawPlc2DFacets()
{

}

void GridRender::setRenderingPiecewiseLinearComplex(
        DelaunayGridGenerator::CommonPlc3D *renderingPlc3D) throw (std::logic_error)
{
    _refToRenderingPlc3D = renderingPlc3D;
    setRenderingMode(MODE_3D);
}

/// \todo need method extraction see _initializePlc3DSegmentsBuffers()
void GridRender::_initializePlc3DNodesBuffers() throw(std::runtime_error)
{
    _renderingNodesVertexPositionBuffer = createBuffer();
    bindBuffer(ARRAY_BUFFER, _renderingNodesVertexPositionBuffer);

    if(!_refToRenderingPlc3D)
        throw std::runtime_error("_loadPlc3DNodesToDevice(), bad pointer to PLC");

    /// \todo It is some kind of magic!
    if(_refToRenderingPlc3D->_nodeList.size() != 0)
    {
        std::string _js;
        _js += "ctx.bufferData(";
        _js += "ctx.ARRAY_BUFFER";
        _js += ",";
        char buf[30];
        /// \todo it may change on x64, see http://habrahabr.ru/company/microsoft/blog/133998/
        /// or https://developer.mozilla.org/en-US/docs/Web/JavaScript/Typed_arrays
        _js += "new Float32Array([";
        for(auto i = _refToRenderingPlc3D->_nodeList.begin();
            i != _refToRenderingPlc3D->_nodeList.end(); ++i)
        {
            _js += (i == _refToRenderingPlc3D->_nodeList.begin() ? "" : ",");
            _js += generic_double_to_str((*i)[0], buf);
            _js += ",";
            _js += generic_double_to_str((*i)[1], buf);
            _js += ",";
            _js += generic_double_to_str((*i)[2], buf);
        }
        _js += "])";
        _js += ",";
        _js += "ctx.STATIC_DRAW";
        _js += ");";

        injectJS(_js);
        GLDEBUG;
    }
}

/// \todo need method extraction see _initializePlc3DNodesBuffers()
void GridRender::_initializePlc3DSegmentsBuffers() throw(std::runtime_error)
{
    // It just the indexes, Nodes should be already loaded
    _renderingSegmentsIndexBuffer = createBuffer();
    bindBuffer(ELEMENT_ARRAY_BUFFER, _renderingSegmentsIndexBuffer);

    if(!_refToRenderingPlc3D)
        throw std::runtime_error("_loadPlc3DNodesToDevice(), bad pointer to PLC");

    if(_refToRenderingPlc3D->_segmentList.size() != 0)
    {
        std::string _js;
        _js += "ctx.bufferData(";
        _js += "ctx.ELEMENT_ARRAY_BUFFER";
        _js += ",";
        char buf[30];
        /// see WGLWidget::renderiv()
        _js += "new Uint16Array([";
        for(auto i = _refToRenderingPlc3D->_segmentList.begin();
            i != _refToRenderingPlc3D->_segmentList.end(); ++i)
        {
            _js += (i == _refToRenderingPlc3D->_segmentList.begin() ? "" : ",");
            _js += itoa((*i).getNodeIndexes()[0], buf, 10);
            _js += ",";
            _js += itoa((*i).getNodeIndexes()[1], buf, 10);
        }
        _js += "])";
        _js += ",";
        _js += "ctx.STATIC_DRAW";
        _js += ");";

        injectJS(_js);
        GLDEBUG;
    }
}

void GridRender::_initializePlc3DFacetsBuffers() throw(std::runtime_error)
{
    /*_renderingFacetsVertexPositionBuffer = createBuffer();
    bindBuffer(ARRAY_BUFFER, _renderingFacetsVertexPositionBuffer);
    bufferDatafv<std::vector<double>::iterator>(
                ARRAY_BUFFER,
                _renderingDataPositions.begin(),
                _renderingDataPositions.end(),
                STATIC_DRAW);

    _renderingFacetsVertexColorBuffer = createBuffer();
    bindBuffer(ARRAY_BUFFER, _renderingFacetsVertexColorBuffer);
    bufferDatafv<std::vector<double>::iterator>(
                ARRAY_BUFFER,
                _renderingDataColors.begin(),
                _renderingDataColors.end(),
                STATIC_DRAW);

    /// \todo element bufer is not needed for now
    //_renderingObjectElementBuffer = createBuffer();
    //bindBuffer(ELEMENT_ARRAY_BUFFER, _renderingObjectElementBuffer);
    //bufferDataiv(ELEMENT_ARRAY_BUFFER, objectElements, count, STATIC_DRAW, DT_UNSIGNED_BYTE);*/
}

void GridRender::_drawPlc3DNodes() throw(std::runtime_error)
{
    if(!_refToRenderingPlc3D)
        throw std::runtime_error("_drawPlc3DNodes(),  bad pointer to PLC");

    bindBuffer(ARRAY_BUFFER, _renderingNodesVertexPositionBuffer);
    vertexAttribPointer(_attributeVertexPosition, 3, FLOAT, false, 0, 0);
    uniform4f(_uniformVertexColor,
            _PlcNodesColor[0],
            _PlcNodesColor[1],
            _PlcNodesColor[2],
            _PlcNodesColor[3]);

    // Find center of all Nodes
    // Move there the center of coordinates
    // Stretch all coordinates into box 1x1x1
    WMatrix4x4 modelMatrix;
    modelMatrix.setToIdentity();

    // Apply matrixes
    uniformMatrix4(_uniformModelMatrix, modelMatrix);
    uniformMatrix4(_uniformViewMatrix, _worldView);
    uniformMatrix4(_uniformClientMatrix, _jsMatrix);
    uniformMatrix4(_uniformProjectionMatrix, _worldProjection);
    drawArrays(POINTS, 0, _refToRenderingPlc3D->_nodeList.size());
}

void GridRender::_drawPlc3DSegments()
{
    // It just the indexes, Nodes should be already loaded

    if(!_refToRenderingPlc3D)
        throw std::runtime_error("_drawPlc3DNodes(),  bad pointer to PLC");

    /*bindBuffer(ARRAY_BUFFER, _renderingNodesVertexPositionBuffer);
    vertexAttribPointer(_attributeVertexPosition, 3, FLOAT, false, 0, 0);*/
    uniform4f(_uniformVertexColor,
            _PlcSegmentsColor[0],
            _PlcSegmentsColor[1],
            _PlcSegmentsColor[2],
            _PlcSegmentsColor[3]);
    bindBuffer(ELEMENT_ARRAY_BUFFER, _renderingSegmentsIndexBuffer);

    // Find center of all Nodes
    // Move there the center of coordinates
    // Stretch all coordinates into box 1x1x1
    WMatrix4x4 modelMatrix;
    modelMatrix.setToIdentity();

    // Apply matrixes
    uniformMatrix4(_uniformModelMatrix, modelMatrix);
    uniformMatrix4(_uniformViewMatrix, _worldView);
    uniformMatrix4(_uniformClientMatrix, _jsMatrix);
    uniformMatrix4(_uniformProjectionMatrix, _worldProjection);
    /// Must be UNSIGNED_SHORT or less
    /// see http://www.khronos.org/opengles/sdk/docs/man/xhtml/glDrawElements.xml
    drawElements(LINES, _refToRenderingPlc3D->_segmentList.size()*2, UNSIGNED_SHORT, 0);
}

void GridRender::_drawPlc3DFacets()
{
    /*bindBuffer(ARRAY_BUFFER, _renderingFacetsVertexPositionBuffer);
    vertexAttribPointer(_vertexPositionAttribute, 3, FLOAT, false, 0, 0);
    bindBuffer(ARRAY_BUFFER, _renderingFacetsVertexColorBuffer);
    vertexAttribPointer(_vertexColorAttribute, 4, FLOAT, false, 0, 0);
    WMatrix4x4 modelMatrix;
    modelMatrix.setToIdentity();
    // Apply matrixes
    uniformMatrix4(_uniformModelMatrix, modelMatrix);
    uniformMatrix4(_uniformViewMatrix, _worldView);
    uniformMatrix4(_uniformClientMatrix, _jsMatrix);
    uniformMatrix4(_uniformProjectionMatrix, _worldProjection);
    drawArrays(TRIANGLES, 0, 3);*/
}

void GridRender::_preloadAllBuffers()
{
    switch(_renderingMode)
    {
        case MODE_3D:
        {
            _initializePlc3DNodesBuffers();
            _initializePlc3DSegmentsBuffers();
            _initializePlc3DFacetsBuffers();

            break;
        }
        case MODE_2D:
        {
            _initializePlc2DNodesBuffers();
            _initializePlc2DFacetsBuffers();
            break;
        }
    }
}

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

    _attributeVertexPosition = getAttribLocation(_shaderProgram, "vrtPos");
    enableVertexAttribArray(_attributeVertexPosition);
    //_vertexColorAttribute = getAttribLocation(_shaderProgram, "vrtClr");
    //enableVertexAttribArray(_vertexColorAttribute);
    _uniformVertexColor = getUniformLocation(_shaderProgram, "vrtClr");

    _uniformModelMatrix = getUniformLocation(_shaderProgram, "mMatrix");
    _uniformViewMatrix = getUniformLocation(_shaderProgram, "vMatrix");
    _uniformClientMatrix = getUniformLocation(_shaderProgram, "cMatrix");
    _uniformProjectionMatrix = getUniformLocation(_shaderProgram, "pMatrix");

    // Preloading buffers
    _preloadAllBuffers();

    disable(DEPTH_TEST);
    disable(CULL_FACE);
    depthFunc(LEQUAL);

    viewport(0, 0, (unsigned) this->width().value(), (unsigned) this->height().value());
    //viewport(0, 0, 800, 600);
    // Setting of matrixes
    _worldView.lookAt(
        0, 0, 5,  // camera default position
        0, 0, 0,    // camera looks at
        0, 1, 0);   // up vector
    // After that, the camera should be at(0,0,0), looks at negative Z,
    // and the sky should be at positeve Y
    _jsMatrix = createJavaScriptMatrix4();
    setJavaScriptMatrix4(_jsMatrix, _worldView);
    setClientSideLookAtHandler(
        _jsMatrix,
        0, 0, 0,    // camera looks at
        0, 1, 0,    // up vector
        0.005, 0.005);  // rotation rate
    _worldProjection.perspective(60, this->width().value()/this->height().value(), 0.01, 10);
    //_worldProjection.perspective(60, 800.0/600.0, 1, 1000);
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

    switch(_renderingMode)
    {
        case MODE_3D:
        {
            // Draw PLC
            _drawPlc3DFacets();
            _drawPlc3DSegments();
            _drawPlc3DNodes();
            break;
        }
        case MODE_2D:
        {
            // Draw PLC
            _drawPlc2DFacets();
            _drawPlc2DNodes();
            break;
        }
    }
}
void GridRender::resizeGL(int width, int height)
{
    viewport(0, 0, width, height);
    //_worldProjection.perspective(60, (double)width / height, 1, 1000);
}

void GridRender::updateGL()
{
    _preloadAllBuffers();
}
