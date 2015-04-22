#include "inclusionpreviewglrender.h"

#include "UI/volumeglrenderrveeditdialog.h"
#include "constants.h"

using namespace UserInterface;

InclusionPreviewGLRender::InclusionPreviewGLRender(
        const RepresentativeVolumeElement *RVE,
        QWidget *pwgt) noexcept:
    VolumeGLRenderBaseController(pwgt),
    _ptrToRVE(RVE)
{
    initializeGLEW();
}

void InclusionPreviewGLRender::loadDataIntoTexture() throw(std::runtime_error)
{
    // Dialog->TabWidget->StackWidget->GaussianFilter->this
    VolumeGLRenderRVEEditDialog* _parent = static_cast<
            VolumeGLRenderRVEEditDialog*>(this->parent()->parent()->parent()->parent());
    int _size = _ptrToRVE->getSize();
    GLbyte *_RGBABuff = new GLbyte[_size * _size * _size * 4];
    if(!_RGBABuff)
        throw(std::runtime_error("loadDataIntoTexture(): "
                                 "can't allocate memory for RVE"));

    for(long i = 0; i<_size * _size * _size; ++i)
    {
        float _kk = i % _size % _size - _size / 2;
        float _jj = i / _size % _size - _size / 2;
        float _ii = i / _size / _size - _size / 2;
        RepresentativeVolumeElement::rotateXYZ(
                    _kk, _jj, _ii,
                    _parent->getFilterRotationOXValue_Inclusion() * M_PI / 180,
                    _parent->getFilterRotationOYValue_Inclusion() * M_PI / 180,
                    _parent->getFilterRotationOZValue_Inclusion() * M_PI / 180);

        float _curRadius = std::sqrt(_kk*_kk /
                _parent->getFilterScaleFactorXValue_Inclusion() /
                _parent->getFilterScaleFactorXValue_Inclusion() +
                _jj*_jj /
                _parent->getFilterScaleFactorYValue_Inclusion() /
                _parent->getFilterScaleFactorYValue_Inclusion() +
                _ii*_ii /
                _parent->getFilterScaleFactorZValue_Inclusion() /
                _parent->getFilterScaleFactorZValue_Inclusion());

        if( _curRadius <= _parent->getMinRadiusValue())
        {
            _RGBABuff[i * 4 + 0] = _parent->getCoreIntensityValue() * 255.0f;
            _RGBABuff[i * 4 + 1] = _parent->getCoreIntensityValue() * 255.0f;
            _RGBABuff[i * 4 + 2] = _parent->getCoreIntensityValue() * 255.0f;
            _RGBABuff[i * 4 + 3] = 255;
        }
        else if(_curRadius <= _parent->getMaxRadiusValue())
        {
            _RGBABuff[i * 4 + 0] = _parent->getCoreIntensityValue() * 127.0f;
            _RGBABuff[i * 4 + 1] = _parent->getCoreIntensityValue() * 127.0f;
            _RGBABuff[i * 4 + 2] = _parent->getCoreIntensityValue() * 127.0f;
            _RGBABuff[i * 4 + 3] = 7;
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
    delete[] _RGBABuff;
}

void InclusionPreviewGLRender::_prepareDataTextureDisplayList() noexcept
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

void InclusionPreviewGLRender::initializeGLEW() throw(std::runtime_error)
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

void InclusionPreviewGLRender::paintGL()
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

InclusionPreviewGLRender::~InclusionPreviewGLRender()
{
    glDeleteLists(_displayListBaseID, 1);
    glDeleteTextures(1, _textureIDs);
}

