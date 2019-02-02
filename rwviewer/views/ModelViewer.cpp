#include "ModelViewer.hpp"
#include <QDebug>
#include <engine/Animator.hpp>
#include <fstream>
#include <objects/GameObject.hpp>
#include <platform/FileHandle.hpp>
#include <widgets/ModelFramesWidget.hpp>
#include "ViewerWidget.hpp"

namespace {
float kAnimationMultiplyer = 1000.f;
}

ModelViewer::ModelViewer(QWidget* parent, Qt::WindowFlags f)
    : ViewerInterface(parent, f), viewing(nullptr) {
    mainSplit = new QSplitter;
    auto mainLayout = new QVBoxLayout;

    frames = new ModelFramesWidget;
    animationSelector = new QComboBox;

    connect(animationSelector, static_cast<void (QComboBox::*)(int)>(
        &QComboBox::currentIndexChanged),
            [&](int index) {
        auto model = static_cast<AnimationListModel*>(
            animationSelector->model());
        this->changeAnimation(model->getAnimations().at(index).second);
    });

    auto leftPanel = new QVBoxLayout;
    leftPanel->addWidget(frames);
    leftPanel->addWidget(animationSelector);

    viewerWidget = createViewer();
    viewerWidget->setMode(ViewerWidget::Mode::Model);

    animationTime = new QSlider(Qt::Horizontal);
    connect(animationTime, &QSlider::sliderMoved,
            [&](int value) {
        if(currentPlayback != animation::kInvalidPlayback) {
            float time = value / kAnimationMultiplyer;
            animationSystem.setTime(currentPlayback, time);
            animationSystem.update(0.f);
        }
    });

    auto animLayout = new QHBoxLayout;
    animLayout->addWidget(animationTime);

    auto viewerArea = new QVBoxLayout;
    viewerArea->addWidget(QWidget::createWindowContainer(viewerWidget));
    viewerArea->addLayout(animLayout);

    auto leftWidget = new QWidget;
    leftWidget->setLayout(leftPanel);
    leftWidget->setFixedWidth(300);
    auto rightWidget = new QWidget;
    rightWidget->setLayout(viewerArea);

    mainSplit->addWidget(leftWidget);
    mainSplit->addWidget(rightWidget);
    mainLayout->addWidget(mainSplit);

    setLayout(mainLayout);
    changeAnimation(nullptr);
}

void ModelViewer::worldChanged() {
    delete animModel;

    auto animations = world()->data->animations;
    AnimationList l { { "", nullptr } };

    std::copy(animations.begin(), animations.end(), std::back_inserter(l));
    animModel = new AnimationListModel(l);

    animationSelector->setModel(animModel);
}

void ModelViewer::changeAnimation(AnimationPtr anim) {
    if (currentPlayback != animation::kInvalidPlayback) {
        animationSystem.removePlayback(currentPlayback);
    }

    animationTime->setEnabled(anim != nullptr);

    if (anim == nullptr) {
        currentPlayback = animation::kInvalidPlayback;
        return;
    }

    animationTime->setRange(0, static_cast<int>(anim->duration *
                                                kAnimationMultiplyer));
    animationTime->setValue(0);

    currentPlayback = animationSystem.createPlayback(
        *anim, viewing->getFrame().get(), animation::Loop::Repeat);

    animationSystem.update(0.f);
}

void ModelViewer::showModel(ClumpPtr model) {
    viewing = model;
    viewerWidget->showModel(model);
    frames->setModel(model);
}

void ModelViewer::showObject(uint16_t object) {
    auto def = world()->data->modelinfo[object].get();
    if (!def) {
        return;
    }

    auto modelName = def->name + ".dff";
    auto textureName = def->name + ".txd";
    auto textures = world()->data->loadTextureArchive(textureName);

    LoaderDFF dffLoader;
    dffLoader.setTextureLookupCallback(
        [&](const std::string& texture, const std::string&) {
            return textures.at(texture);
        });

    auto file = world()->data->index.openFile(modelName);
    if (!file.data) {
        RW_ERROR("Couldn't load " << modelName);
        return;
    }
    showModel(dffLoader.loadFromMemory(file));
}
