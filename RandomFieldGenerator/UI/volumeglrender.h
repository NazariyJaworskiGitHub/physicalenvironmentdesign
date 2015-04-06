#ifndef VOLUMEGLRENDER_H
#define VOLUMEGLRENDER_H

#include "UI/volumeglrenderbasecontroller.h"
#include "UI/volumeglrendereditdialog.h"

namespace UserInterface
{
    class VolumeGLRender : public VolumeGLRenderBaseController
    {
        Q_OBJECT

        friend class UserInterface::VolumeGLRenderEditDialog;

        // set on construction, can't be changed
        private: const int _RVEDiscretesize;
        private: const float * const _ptrToRVEdata = nullptr;
        private: const float * const _ptrToRVEpotentialField = nullptr;

        private: float _innerCutLevel = 0.5f;
        public : float getInnerCutLevel() const noexcept {return _innerCutLevel;}
        public : void setInnerCutLevel(float newCutLevel) noexcept {_innerCutLevel = newCutLevel;}

        private: void _grayscaleToRainbow(const float gray, int &r, int &g, int &b) noexcept;

        private: GLuint _firstDisplayListID; // just increment for next list id
        private: GLuint _textureIDs[2];
        private: void _loadFieldIntoTexture() throw(std::runtime_error);
        private: void _prepareTextureDisplayList() noexcept;
        private: void _loadPotentialFieldIntoTexture() throw(std::runtime_error);
        private: void _preparePotentialTextureDisplayList() noexcept;
        private: unsigned char _potentialFieldAlphaLevel = 120;

        private: float _minPotentialValue;  // see constructor
        private: float _maxPotentialValue;
        private: const float _minPotentialValueBackup = 0;
        private: const float _maxPotentialValueBackup = 0;
        private: void _drawRainbowTable() noexcept;

        public : virtual void keyPressEvent(QKeyEvent *e) override;

        public : virtual void initializeGLEW();
        public : virtual void paintGL() override;

        private: QAction* _actionEdit;

        public : VolumeGLRender(
                const int RVEDiscreteSize,
                const float * ptrToRVEData,
                const float * ptrToRVEPotentialField,
                QWidget *pwgt) noexcept;

        public : ~VolumeGLRender();

        private: Q_SLOT void slot_createEditDialog();
    };
}

#endif // VOLUMEGLRENDER_H
