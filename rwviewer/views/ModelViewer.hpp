#ifndef _MODELVIEWER_HPP_
#define _MODELVIEWER_HPP_
#include <engine/GameData.hpp>
#include <engine/GameWorld.hpp>
#include <animation/AnimationSystem.hpp>

#include "ViewerInterface.hpp"
#include <models/AnimationListModel.hpp>

#include <QComboBox>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QSlider>
#include <QSplitter>
#include <QTreeView>
#include <QVBoxLayout>

class ViewerWidget;
class Clump;
class ModelFramesWidget;

class ModelViewer : public ViewerInterface {
    Q_OBJECT

    ClumpPtr viewing;

    QSplitter* mainSplit;
    ViewerWidget* viewerWidget;
    QSlider* animationTime;

    ModelFramesWidget* frames;
    QComboBox* animationSelector;
    AnimationListModel* animModel = nullptr;

    animation::AnimationSystem animationSystem;
    animation::PlaybackID currentPlayback = animation::kInvalidPlayback;


public:
    ModelViewer(QWidget* parent = 0, Qt::WindowFlags f = 0);

    void worldChanged() override;

    void changeAnimation(AnimationPtr);

public slots:

    /**
     * Display a raw model
     */
    void showModel(ClumpPtr model);

    /**
     * Display a game object's model
     */
    void showObject(uint16_t object);
};

#endif
