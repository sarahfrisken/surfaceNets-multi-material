//
// glView.c
//
// OpenGL View 
//  + Controls the OpenGL window and OpenGl rendering of a SurfaceNet model

#include "glView.h"
#include "MMSurfaceNet.h"
#include "MMGeometryGL.h"
#include "glShaders.h"

#include <QMouseEvent>
#include <math.h>
 
//
// Public
//
GLView::GLView(QWidget *parent)
	: QOpenGLWidget(parent),
	m_pGeometry(NULL),
	m_numIndices(0),
	m_xRot(0),
	m_yRot(0),
	m_zRot(0),
	m_scale(0),
	m_dx(0),
	m_dy(0),
	m_dz(0)
{
}

GLView::~GLView()
{
	delete m_pGeometry;
	cleanupBufers();
	makeCurrent();
	delete program;
	doneCurrent();
}

void GLView::updateRenderParameters(std::vector<QColor> colors, std::vector<bool> isVisible)
{
	// Clear the color array
	int idxArray = 0;
	for (int i = 0; i < maxNumColors; i++) {
		surfaceColorArray[idxArray++] = 0;
		surfaceColorArray[idxArray++] = 0;
		surfaceColorArray[idxArray++] = 0;
		surfaceColorArray[idxArray++] = 0;
	}

	// Set color and visibility for each material
	idxArray = 0;
	for (int i = 0; i < colors.size(); i++) {
		float alpha = isVisible[i] ? 1.0 : 0.0;
		surfaceColorArray[idxArray++] = colors[i].redF();
		surfaceColorArray[idxArray++] = colors[i].greenF();
		surfaceColorArray[idxArray++] = colors[i].blueF();
		surfaceColorArray[idxArray++] = alpha;
	}
	update();
}

void GLView::makeGeometry(MMSurfaceNet *surfaceNet)
{
	// Make geometry from the SurfaceNet. Store the origin and size for fast access
	// during rendering
	delete m_pGeometry;
	m_pGeometry = new MMGeometryGL(surfaceNet);
	m_pGeometry->origin(m_origin);
	m_pGeometry->maxSize(m_size);

	// Create GL buffers for rendering
	indexBuffer = makeIndexBuffer(m_pGeometry->indices(), m_pGeometry->numIndices());
	vertexBuffer = makeVertexBuffer(m_pGeometry->vertices(), m_pGeometry->numVertices());
	m_numIndices = m_pGeometry->numIndices();
}

void GLView::reset()
{
	// Reset the view
	m_xRot = 0;
	m_yRot = 0;
	m_zRot = 0;
	m_scale = 1.0;
	m_dx = 0.0;
	m_dy = 0.0;
	m_dz = 0.0;
}

//
// Protected
//
void GLView::mousePressEvent(QMouseEvent *e)
{
	// Save mouse press position
	m_lastMousePos = e->pos();
}

void GLView::mouseMoveEvent(QMouseEvent *event)
{
	int dx = event->x() - m_lastMousePos.x();
	int dy = event->y() - m_lastMousePos.y();

	if (event->buttons() & Qt::LeftButton) {
		setXRotation(m_xRot + 0.5f * dy);
		setYRotation(m_yRot + 0.5f * dx);
	}
	else if (event->buttons() & Qt::RightButton) {
		setScale(m_scale + 0.002 * dx);
	}
	else if (event->buttons() & Qt::MiddleButton) {
		QSize glWindowSize = size();
		float x = dx / ((float)glWindowSize.width());
		float y = -dy / ((float)glWindowSize.height());
		float z = 0;
		setTranslate(m_dx + x, m_dy + y, m_dz + z);
	}
	m_lastMousePos = event->pos();
}

void GLView::mouseDoubleClickEvent(QMouseEvent *event)
{
	if (event->buttons() & Qt::LeftButton) {
		setXRotation(0);
		setYRotation(0);
		setScale(1.0);
		setTranslate(0, 0, 0);
	}
}

void GLView::initializeGL()
{
	makeCurrent();
	initializeOpenGLFunctions();

	// To set the projection matrix
	QSize glWindowSize = size();
	resizeGL(glWindowSize.width(), glWindowSize.height());

	// Set up OpenGL
	program = 0;
	glClearColor(0, 0, 0, 1);
	initShaders(vertexShader, fragmentShader);
	doneCurrent();
}

void GLView::resizeGL(int w, int h)
{
	projMatrix.setToIdentity();
	projMatrix.perspective(45.0f, GLfloat(w) / h, 0.01f, 100.0f);
}


void GLView::paintGL()
{
	render();
}

//
// Private
//
void GLView::cleanupBufers()
{
	makeCurrent();
	if (vertexBuffer) {
		vertexBuffer->destroy();
		delete vertexBuffer;
	}
	if (indexBuffer) {
		indexBuffer->destroy();
		delete indexBuffer;
	}
	doneCurrent();
}

//
// GL rendering utilities
//
void GLView::initShaders(const char *vertexShader, const char *fragmentShader)
{
	// Compile shaders
	program = new QOpenGLShaderProgram;
	program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShader);
	program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShader);

	// Link shader pipeline
	if (!program->link())
		close();

	// Bind shader pipeline for use
	if (!program->bind())
		close();
}

void GLView::render()
{
	if (m_pGeometry == NULL) {
		return;
	}

	// Set OpenGL state
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	// Calculate model and view transforms
	QMatrix4x4 view;
	view.setToIdentity();
	view.translate(0, 0, -4);
	view.translate(m_dx, m_dy, m_dz);
	view.scale(m_scale);
	view.rotate(180.0f - m_xRot, 1, 0, 0);
	view.rotate(m_yRot, 0, 1, 0);
	view.rotate(m_zRot, 0, 0, 1);

	QMatrix4x4 model;
	model.setToIdentity();
	model.translate(-1, -1, -1); // Translate to [-1,-1,-1] to [1,1,1] cube
	int maxSize = (m_size[0] > m_size[1]) ? m_size[0] : m_size[1];
	maxSize = (maxSize > m_size[2]) ? maxSize : m_size[2];
	model.scale(2.0f / (float)maxSize);	// Scale to [0,0,0] to [2,2,2] cube
	model.translate(-m_origin[0], -m_origin[1], -m_origin[2]); // Translate to origin

	// Set model, view projection matrices and light direction
	program->bind();
	int projMatrixLoc = program->uniformLocation("u_projMatrix");
	int modelMatrixLoc = program->uniformLocation("u_modelMatrix");
	int viewMatrixLoc = program->uniformLocation("u_viewMatrix");
	int normalMatrixLoc = program->uniformLocation("u_normalMatrix");
	int lightPosLoc = program->uniformLocation("u_lightPos");

	program->setUniformValue(projMatrixLoc, projMatrix);
	program->setUniformValue(modelMatrixLoc, model);
	program->setUniformValue(viewMatrixLoc, view);
	QMatrix3x3 normalMatrix= view.normalMatrix();
	program->setUniformValue(normalMatrixLoc, normalMatrix);
	QVector3D lightPosition = QVector3D(-45, 45, -100);
	program->setUniformValue(lightPosLoc, lightPosition);

	// Draw the surface net
	drawGeometry(program);
	program->release();
}

QOpenGLBuffer *GLView::makeIndexBuffer(GLuint *indices, int numIndices)
{
	QOpenGLBuffer *buffer = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
	buffer->create();
	buffer->bind();
	buffer->allocate(indices, numIndices * sizeof(GLuint));
	buffer->release();
	return buffer;
}

QOpenGLBuffer *GLView::makeVertexBuffer(GLfloat *vertices, int numVertices)
{
	QOpenGLBuffer *buffer = new QOpenGLBuffer;
	buffer->create();
	int numFloatsPerVertex = sizeof(MMGeometryGL::GLVertex) / sizeof(float);
	buffer->bind();
	buffer->allocate(vertices, numVertices * numFloatsPerVertex * sizeof(GLfloat));
	buffer->release();
	return buffer;
}

void GLView::resetVertexBuffer(QOpenGLBuffer *buffer, GLfloat *vertices, int numVertices)
{
	int numFloatsPerVertex = sizeof(MMGeometryGL::GLVertex) / sizeof(float);
	buffer->bind();
	auto ptr = buffer->map(QOpenGLBuffer::WriteOnly);
	memcpy(ptr, vertices, numVertices * numFloatsPerVertex * sizeof(GLfloat));
	buffer->unmap();
	buffer->release();
}

void GLView::drawGeometry(QOpenGLShaderProgram *program)
{
	if (m_pGeometry == NULL) {
		return;
	}

	int colorMapLocation = program->uniformLocation("colorMap");
	program->setUniformValueArray(colorMapLocation, surfaceColorArray, 256, 4);

	// Tell OpenGL which VBOs to use
	vertexBuffer->bind();

	qintptr offset = 0;

	// Tell OpenGL programmable pipeline how to locate vertex position data
	int numFloatsPerVertex = sizeof(MMGeometryGL::GLVertex) / sizeof(float);
	int vertexSize = numFloatsPerVertex * sizeof(GLfloat);
	int vertexLocation = program->attributeLocation("a_position");
	program->enableAttributeArray(vertexLocation);
	program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, vertexSize);

	// Offset for normal coordinate
	offset += 3 * sizeof(float);

	// Tell OpenGL programmable pipeline how to locate vertex normal data
	int normalLocation = program->attributeLocation("a_normal");
	program->enableAttributeArray(normalLocation);
	program->setAttributeBuffer(normalLocation, GL_FLOAT, offset, 3, vertexSize);

	// Offset for texture coordinate
	offset += 3 * sizeof(float);

	// Tell OpenGL programmable pipeline how to locate texture coordinate data
	int texCoordLocation = program->attributeLocation("a_texcoord");
	program->enableAttributeArray(texCoordLocation);
	program->setAttributeBuffer(texCoordLocation, GL_FLOAT, offset, 2, vertexSize);

	vertexBuffer->release();

	// Draw surface net using indices
	indexBuffer->bind();
	glDrawElements(GL_TRIANGLES, m_numIndices, GL_UNSIGNED_INT, 0);
	indexBuffer->release();
}

//
// 3D view control
//
static void normalizeAngle(int &angle)
{
	while (angle < 0)
		angle += 360.0f;
	while (angle > 360.0f)
		angle -= 360.0f;
}

void GLView::setXRotation(int angle)
{
	normalizeAngle(angle);
	if (angle != m_xRot) {
		m_xRot = angle;
		update();
	}
}

void GLView::setYRotation(int angle)
{
	normalizeAngle(angle);
	if (angle != m_yRot) {
		m_yRot = angle;
		update();
	}
}

void GLView::setZRotation(int angle)
{
	normalizeAngle(angle);
	if (angle != m_zRot) {
		m_zRot = angle;
		update();
	}
}

void GLView::setScale(float scale)
{
	float minScale = 0.1f;
	float maxScale = 100.0f;
	if (scale < minScale) scale = minScale;
	if (scale > maxScale) scale = maxScale;
	if (scale != m_scale) {
		m_scale = scale;
		update();
	}
}
void GLView::setTranslate(float dx, float dy, float dz)
{
	m_dx = dx;
	m_dy = dy;
	m_dz = dz;
	update();
}
