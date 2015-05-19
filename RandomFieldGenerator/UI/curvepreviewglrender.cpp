#include "curvepreviewglrender.h"

#include "inclusionpreviewglrender.h"

#include "UI/volumeglrenderrveeditdialog.h"
#include "constants.h"

using namespace UserInterface;

CurvePreviewGLRender::CurvePreviewGLRender(
        const RepresentativeVolumeElement *RVE,
        QWidget *pwgt) noexcept:
    VolumeGLRenderBaseController(pwgt),
    _ptrToRVE(RVE)
{
    setBoundingBoxRepresentationSize(_ptrToRVE->getRepresentationSize());

    initializeGLEW();
}

void CurvePreviewGLRender::loadDataIntoTexture() throw(std::runtime_error)
{
    // Dialog->TabWidget->StackWidget->GaussianFilter->this
    VolumeGLRenderRVEEditDialog* _parent = static_cast<
            VolumeGLRenderRVEEditDialog*>(this->parent()->parent()->parent()->parent());
    int _size = _ptrToRVE->getSize();
    GLbyte *_RGBABuff = new GLbyte[_size * _size * _size * 4];
    if(!_RGBABuff)
        throw(std::runtime_error("loadDataIntoTexture(): "
                                 "can't allocate memory for RVE"));

    int curveOrder = _parent->getCurveOrder();
    int curveApproximationPoints = _parent->getCurveApproximationPoints();
    int discreteLength = _parent->getFiberLength();
    float minScale = _parent->getMinimalScale();
    int curveRadius = _parent->getFiberRadius();
    float pathDeviation = _parent->getPathDeviation();
    float transitionLayerSize = _parent->getTransitionLayerValue_Fiber();
    float rotationOX = _parent->getFilterRotationOXValue_Fiber();
    float rotationOY = _parent->getFilterRotationOYValue_Fiber();
    float rotationOZ = _parent->getFilterRotationOZValue_Fiber();
    float coreValue = _parent->getCoreIntensityValue_Fiber();

    float *_controlPolygonPoints = new float[curveOrder*3];
    if(!_controlPolygonPoints)
        throw(std::runtime_error("loadDataIntoTexture():"
                                 "can't allocate memory for temporary storage.\n"));
    for(int k=0; k<curveOrder; ++k)
    {
        _controlPolygonPoints[k*3+0] = (-0.5f + k/(curveOrder-1.0f)) * discreteLength;
        _controlPolygonPoints[k*3+1] = MathUtils::rand<float>(
                    -pathDeviation, pathDeviation) * discreteLength;
        _controlPolygonPoints[k*3+2] = MathUtils::rand<float>(
                    -pathDeviation, pathDeviation) * discreteLength;
    }

    float *_curveAproximation = new float[curveApproximationPoints*3];
    if(!_curveAproximation)
        throw(std::runtime_error("loadDataIntoTexture():"
                                 "can't allocate memory for temporary storage.\n"));
    for(int k=0; k<curveApproximationPoints; ++k)
    {
        _curveAproximation[k*3+0] = RepresentativeVolumeElement::_BezierCurve(
                    0, curveOrder, _controlPolygonPoints,
                    k, curveApproximationPoints);
        _curveAproximation[k*3+1] = RepresentativeVolumeElement::_BezierCurve(
                    1, curveOrder, _controlPolygonPoints,
                    k, curveApproximationPoints);
        _curveAproximation[k*3+2] = RepresentativeVolumeElement::_BezierCurve(
                    2, curveOrder, _controlPolygonPoints,
                    k, curveApproximationPoints);
    }

    for(long i = 0; i<_size * _size * _size; ++i)
    {
        float _kk = i % _size % _size - _size / 2;
        float _jj = i / _size % _size - _size / 2;
        float _ii = i / _size / _size - _size / 2;
        RepresentativeVolumeElement::rotateXYZ(
                    _kk, _jj, _ii,
                    rotationOX * M_PI / 180,
                    rotationOY * M_PI / 180,
                    rotationOZ * M_PI / 180);
        float _minDist = RepresentativeVolumeElement::_distanceToBezierSamplePoint(
                    _kk, _jj, _ii, 0, _curveAproximation);
        int _sampleIndexA = 0;
        int _sampleIndexB = 1;
        for(int s=1; s<curveApproximationPoints; ++s)
        {
            float _curDist = RepresentativeVolumeElement::_distanceToBezierSamplePoint(
                        _kk, _jj, _ii, s, _curveAproximation);
            if(_curDist < _minDist)
            {
                _minDist = _curDist;
                _sampleIndexA = s;
            }
        }
        if(_sampleIndexA != 0 && _sampleIndexA != curveApproximationPoints-1)
        {
            float _minDistLeft = RepresentativeVolumeElement::_distanceToBezierSamplePoint(
                        _kk, _jj, _ii, _sampleIndexA-1, _curveAproximation);
            float _minDistRight = RepresentativeVolumeElement::_distanceToBezierSamplePoint(
                        _kk, _jj, _ii, _sampleIndexA+1, _curveAproximation);
            if(_minDistLeft < _minDistRight)
                _sampleIndexB = _sampleIndexA-1;
            else _sampleIndexB = _sampleIndexA+1;
        }
        else if(_sampleIndexA == 0) _sampleIndexB = 1;
        else  _sampleIndexB = _sampleIndexA-1;
        if(!((_sampleIndexA == 0 || _sampleIndexA == curveApproximationPoints-1) &&
             RepresentativeVolumeElement::_projectionLength(
                 _kk, _jj, _ii,
                 _curveAproximation[_sampleIndexA*3 + 0],
                 _curveAproximation[_sampleIndexA*3 + 1],
                 _curveAproximation[_sampleIndexA*3 + 2],
                 _curveAproximation[_sampleIndexB*3 + 0],
                 _curveAproximation[_sampleIndexB*3 + 1],
                 _curveAproximation[_sampleIndexB*3 + 2]) < 0.0f))
        {
            _minDist = RepresentativeVolumeElement::_distanceToLine(
                        _kk, _jj, _ii,
                        _curveAproximation[_sampleIndexA*3 + 0],
                    _curveAproximation[_sampleIndexA*3 + 1],
                    _curveAproximation[_sampleIndexA*3 + 2],
                    _curveAproximation[_sampleIndexB*3 + 0],
                    _curveAproximation[_sampleIndexB*3 + 1],
                    _curveAproximation[_sampleIndexB*3 + 2]);
        }
        _minDist = std::sqrt(_minDist);
        if(_minDist <= curveRadius*(1.0f-transitionLayerSize)*minScale)
        {
            _RGBABuff[i * 4 + 0] = coreValue * 255.0f;
            _RGBABuff[i * 4 + 1] = coreValue * 255.0f;
            _RGBABuff[i * 4 + 2] = coreValue * 255.0f;
            _RGBABuff[i * 4 + 3] = 255;
        }
        else if(_minDist <= curveRadius*minScale)
        {
            _RGBABuff[i * 4 + 0] = coreValue * 191.0f;
            _RGBABuff[i * 4 + 1] = coreValue * 191.0f;
            _RGBABuff[i * 4 + 2] = coreValue * 191.0f;
            _RGBABuff[i * 4 + 3] = 15;
        }
        else if(_minDist <= curveRadius*(1.0f-transitionLayerSize))
        {
            _RGBABuff[i * 4 + 0] = coreValue * 127.0f;
            _RGBABuff[i * 4 + 1] = coreValue * 127.0f;
            _RGBABuff[i * 4 + 2] = coreValue * 127.0f;
            _RGBABuff[i * 4 + 3] = 7;
        }
        else if(_minDist <= curveRadius)
        {
            _RGBABuff[i * 4 + 0] = coreValue * 63.0f;
            _RGBABuff[i * 4 + 1] = coreValue * 63.0f;
            _RGBABuff[i * 4 + 2] = coreValue * 63.0f;
            _RGBABuff[i * 4 + 3] = 3;
        }
    }

    glBindTexture(GL_TEXTURE_3D, _textureIDs[0]);
    // see https://www.opengl.org/sdk/docs/man3/xhtml/glTexImage3D.xml
    glTexImage3D(
                GL_TEXTURE_3D,      // target, copy data to device
                0,                  // level
                GL_RGBA,            // internalFormat
                _size,              // width
                _size,              // height
                _size,              // depth
                0,                  // border
                GL_RGBA,            // format
                GL_UNSIGNED_BYTE,   // type
                _RGBABuff           // ptr to data
                );
    delete [] _RGBABuff;
    delete [] _controlPolygonPoints;
    delete [] _curveAproximation;
}

void CurvePreviewGLRender::_prepareDataTextureDisplayList() noexcept
{
    glNewList(_displayListBaseID, GL_COMPILE);
    glEnable(GL_TEXTURE_3D);
    glBindTexture( GL_TEXTURE_3D, _textureIDs[0]);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);  // material color (for lightning)
    glBegin(GL_QUADS);
    for (float _fIndx = 0.0f; _fIndx <= 1.0f; _fIndx += 1.0 / _ptrToRVE->getSize())
    {
        glNormal3f(0.0f, 0.0f, 1.0f);

        glTexCoord3f(0.0f, 0.0f, _fIndx);   glVertex3f(0.0f, 0.0f, _fIndx);
        glTexCoord3f(1.0f, 0.0f, _fIndx);   glVertex3f(1.0f, 0.0f, _fIndx);
        glTexCoord3f(1.0f, 1.0f, _fIndx);   glVertex3f(1.0f, 1.0f, _fIndx);
        glTexCoord3f(0.0f, 1.0f, _fIndx);   glVertex3f(0.0f, 1.0f, _fIndx);

        //////////////////////////////////
        glNormal3f(0.0f, 1.0f, 0.0f);

        glTexCoord3f(1.0f, _fIndx, 0.0f);   glVertex3f(1.0f, _fIndx, 0.0f);
        glTexCoord3f(0.0f, _fIndx, 0.0f);   glVertex3f(0.0f, _fIndx, 0.0f);
        glTexCoord3f(0.0f, _fIndx, 1.0f);   glVertex3f(0.0f, _fIndx, 1.0f);
        glTexCoord3f(1.0f, _fIndx, 1.0f);   glVertex3f(1.0f, _fIndx, 1.0f);

        //////////////////////////////////
        glNormal3f(1.0f, 0.0f, 0.0f);

        glTexCoord3f(_fIndx, 0.0f, 0.0f);   glVertex3f(_fIndx, 0.0f, 0.0f);
        glTexCoord3f(_fIndx, 1.0f, 0.0f);   glVertex3f(_fIndx, 1.0f, 0.0f);
        glTexCoord3f(_fIndx, 1.0f, 1.0f);   glVertex3f(_fIndx, 1.0f, 1.0f);
        glTexCoord3f(_fIndx, 0.0f, 1.0f);   glVertex3f(_fIndx, 0.0f, 1.0f);
    }
    glEnd();
    glDisable(GL_TEXTURE_3D);
    glEndList();
}

void CurvePreviewGLRender::initializeGLEW() throw(std::runtime_error)
{
    this->context()->makeCurrent();
    auto _res = glewInit();
    if(_res != GLEW_OK)
        throw(std::runtime_error(std::string("FATAL: GLEW: ") + reinterpret_cast<const char*>(
                                     glewGetErrorString(_res))));
    glGenTextures(1, _textureIDs);
    _displayListBaseID = glGenLists(1);
    loadDataIntoTexture();
    _prepareDataTextureDisplayList();
}

void CurvePreviewGLRender::paintGL()
{
    glClearColor(
                _BackgroundColor.redF(),
                _BackgroundColor.greenF(),
                _BackgroundColor.blueF(),
                _BackgroundColor.alphaF());

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf((_mWorld * _mControl).constData());

    glScalef(_Zoom, _Zoom, _Zoom);
    glTranslatef(-0.5f, -0.5f, -0.5f);

    _drawBoundingBox();

    //if(!_isPressed)
    {
        glCallList(_displayListBaseID);
    }
}

CurvePreviewGLRender::~CurvePreviewGLRender()
{
    glDeleteLists(_displayListBaseID, 1);
    glDeleteTextures(1, _textureIDs);
}

