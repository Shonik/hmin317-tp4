/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "mainwidget.h"

#include <QMouseEvent>

#include <math.h>

MainWidget::MainWidget(int fps, SEASON season, QWidget *parent) :
    QOpenGLWidget(parent),
    geometries(0),
    angularSpeed(0.5),
    rotationAxis(0.0, 0.0, 1),
    m_fps(fps),
    m_season(season),
    m_calendar(new QTimer(this)),
    m_nbParticules(0)
{
    this->grabKeyboard();
    connect(m_calendar, SIGNAL(timeout()), this, SLOT(updateSeason()));
    m_calendar->start(5000);
}

MainWidget::~MainWidget()
{
    // Make sure the context is current when deleting the texture
    // and the buffers.
    makeCurrent();
    delete textureSummer;
    delete textureSpring;
    delete textureFall;
    delete textureWinter;
    delete geometries;
    doneCurrent();
}


void MainWidget::timerEvent(QTimerEvent *)
{
    rotation = QQuaternion::fromAxisAndAngle(rotationAxis, angularSpeed) * rotation;

    update();

}

void MainWidget::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {

        case Qt::Key_Up:
            angularSpeed += 0.1;
            update();
            break;
        case Qt::Key_Down:
            angularSpeed -= 0.1;
            update();
            break;
    }

    e->accept();
}

void MainWidget::keyReleaseEvent(QKeyEvent *e)
{

}

void MainWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glClearColor(0, 0, 0, 1);

    initShaders();
    initTextures();

    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Enable back face culling
    glEnable(GL_CULL_FACE);

    geometries = new GeometryEngine;

    // Use QBasicTimer because its faster than QTimer
    // 1000 ms = 1 sec
    // on divise donc par le nombre de frames par seconde que l'on souhaite
    timer.start(1000/m_fps, this);
}

void MainWidget::initShaders()
{
    // Compile vertex shader
    if (!program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/vshader.glsl"))
        close();

    // Compile fragment shader
    if (!program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/fshader.glsl"))
        close();

    // Link shader pipeline
    if (!program.link())
        close();

    // Bind shader pipeline for use
    if (!program.bind())
        close();
}

void MainWidget::initTextures()
{

    textureSummer = new QOpenGLTexture(QImage(":/summer.jpg"));
    textureSpring = new QOpenGLTexture(QImage(":/spring.jpg"));
    textureFall = new QOpenGLTexture(QImage(":/fall.jpg"));
    textureWinter = new QOpenGLTexture(QImage(":/snow.jpg"));

    // Set nearest filtering mode for texture minification
    textureSummer->setMinificationFilter(QOpenGLTexture::Nearest);
    textureSpring->setMinificationFilter(QOpenGLTexture::Nearest);
    textureFall->setMinificationFilter(QOpenGLTexture::Nearest);
    textureWinter->setMinificationFilter(QOpenGLTexture::Nearest);

    // Set bilinear filtering mode for texture magnification
    textureSummer->setMagnificationFilter(QOpenGLTexture::Linear);
    textureSpring->setMagnificationFilter(QOpenGLTexture::Linear);
    textureFall->setMagnificationFilter(QOpenGLTexture::Linear);
    textureWinter->setMagnificationFilter(QOpenGLTexture::Linear);

    // Wrap texture coordinates by repeating
    // f.ex. texture coordinate (1.1, 1.2) is same as (0.1, 0.2)
    textureSummer->setWrapMode(QOpenGLTexture::Repeat);
    textureSpring->setWrapMode(QOpenGLTexture::Repeat);
    textureFall->setWrapMode(QOpenGLTexture::Repeat);
    textureWinter->setWrapMode(QOpenGLTexture::Repeat);
}

void MainWidget::updateSeason()
{
    switch (m_season) {
    case summer:
        changeSeason(fall);
        break;
    case fall:
        changeSeason(winter);
        break;
    case winter:
        changeSeason(spring);
        break;
    case spring:
        changeSeason(summer);
        break;
    }
}

void MainWidget::changeSeason(SEASON season)
{
    switch (season) {
    case summer:

        m_season = summer;
        m_nbParticules = 0;
        qDebug() << "summer";
        break;

    case fall:

        m_season = fall;
        qDebug() << "fall";

        if (m_nbParticules == 0)
        {
            m_nbParticules = 10000;
            for(int i = 0; i < m_nbParticules; ++i)
            {
                m_particules[i] = QVector3D(randMax(-1, 1), randMax(-1, 1), randMax(0, 1));
            }
        }

        break;

    case spring:

        m_season = spring;
        m_nbParticules = 0;
        qDebug() << "spring";
        break;

    case winter:

        m_season = winter;
        qDebug() << "winter";

        if (m_nbParticules == 0)
        {
            m_nbParticules = 10000;
            for(int i = 0; i < m_nbParticules; ++i)
            {
                m_particules[i] = QVector3D(randMax(-1, 1), randMax(-1, 1), randMax(0, 1));
            }
        }

        break;

    }

}

void MainWidget::resizeGL(int w, int h)
{
    // Calculate aspect ratio
    qreal aspect = qreal(w) / qreal(h ? h : 1);

    // Set near plane to 1.0, far plane to 10.0, field of view 45 degrees
    //const qreal zNear = 1.0, zFar = 10.0, fov = 35.0;
    const qreal zNear = 0.1, zFar = 100.0, fov = 45.0;

    // Reset projection
    projection.setToIdentity();

    // Set perspective projection
    projection.perspective(fov, aspect, zNear, zFar);
}
//! [5]

void MainWidget::paintGL()
{
    // Clear color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //texture->bind();

//! [6]
    // Calculate model view transformation
    QMatrix4x4 matrix;

    matrix.translate(0.0, 0.0, -5.0);

    QQuaternion framing = QQuaternion::fromAxisAndAngle(QVector3D(1,0,0),-45.0);
    matrix.rotate(framing);

//    matrix.translate(0.0, 0.0, 0.0);

    // QVector3D eye = QVector3D(0.0,0.5,-5.0);
    // QVector3D center = QVector3D(0.0,0.0,2.0);
    // QVector3D up = QVector3D(-1,0,0);
    // matrix.lookAt(eye,center,up);

    matrix.rotate(rotation);


    // Set modelview-projection matrix
    program.setUniformValue("mvp_matrix", projection * matrix);

    //program.setUniformValue("texture", 0);
    switch (m_season) {
    case summer:
        textureSummer->bind(0);
        program.setUniformValue("textureSummer", 0);
        break;
    case fall:
        textureFall->bind(0);
        program.setUniformValue("textureFall", 0);
        break;
    case spring:
        textureSpring->bind(0);
        program.setUniformValue("textureSpring", 0);
        break;
    case winter:
        textureWinter->bind(0);
        program.setUniformValue("textureWinter", 0);
        break;
    default:
        break;
    }

    geometries->drawPlaneGeometry(&program);

    if (m_nbParticules > 0)
    {
        glBegin(GL_POINTS);

        for(int i= 0; i < m_nbParticules; ++i)
        {
            m_particules[i].setZ(m_particules[i].z() - 0.01f);

            if (m_particules[i].z() < 0)
            {
                m_particules[i] = QVector3D(randMax(-1, 1), randMax(-1, 1), randMax(0, 1));
            }
            glVertex3f(m_particules[i].x(), m_particules[i].y(), m_particules[i].z());
        }

        glEnd();
    }

}

float MainWidget::randMax(int min, int max)
{
    return ((float(rand()) / float(RAND_MAX)) * (max - min)) + min;
}
