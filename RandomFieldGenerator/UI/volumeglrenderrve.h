#ifndef VOLUMEGLRENDERRVE_H
#define VOLUMEGLRENDERRVE_H

#include "UI/volumeglrenderbasecontroller.h"
#include "UI/volumeglrenderrveeditdialog.h"
#include "representativevolumeelement.h"

namespace UserInterface
{
    class VolumeGLRenderRVE : public VolumeGLRenderBaseController
    {
        Q_OBJECT

        friend class UserInterface::VolumeGLRenderRVEEditDialog;

        private: RepresentativeVolumeElement* _ptrToRVE;

        private: int _MinRadiusBackup;
        private: int _MaxRadiusBackup;

        private: float _FilterScaleFactorXBackup = 1.0f;
        private: float _FilterScaleFactorYBackup = 1.0f;
        private: float _FilterScaleFactorZBackup = 1.0f;

        private: float _FilterRotationOXBackup = 0.0f;
        private: float _FilterRotationOYBackup = 0.0f;
        private: float _FilterRotationOZBackup = 0.0f;

        private: float _TransitionLayerBackup = 0.5f;
        private: float _CoreIntensityBackup = 1.0f;

        private: float _innerBottomCutLevel = 0.0f; // min
        public : float getInnerBottomCutLevel() const noexcept {return _innerBottomCutLevel;}
        public : void setInnerBottomCutLevel(float newBottomCutLevel) noexcept {
            _innerBottomCutLevel = newBottomCutLevel;}

        private: float _innerTopCutLevel = 1.0f;    // max
        public : float getInnerTopCutLevel() const noexcept {return _innerTopCutLevel;}
        public : void setInnerTopCutLevel(float newTopCutLevel) noexcept {
            _innerTopCutLevel = newTopCutLevel;}

        /// \todo need refactoring, make some texture manager
        private: GLuint _displayListBaseID; // just increment for next list id
        private: GLuint _textureIDs[1];     // it is array because unified approach to texturing
        private: void _loadRVEDataIntoTexture() throw(std::runtime_error);
        private: void _prepareRVEDataTextureDisplayList() noexcept;

        public : virtual void initializeGLEW() throw(std::runtime_error);
        public : virtual void paintGL() override;

        private: QAction* _actionEdit;

        public : VolumeGLRenderRVE(
                RepresentativeVolumeElement *RVE,
                QWidget *pwgt) noexcept;

        public :~VolumeGLRenderRVE();

        private: Q_SLOT void slot_createEditDialog();
    };
}

#endif // VOLUMEGLRENDERRVE_H
