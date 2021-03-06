/** \file App.cpp */
#include "App.h"

void	makeCylinder(float rad, float height);


// Tells C++ to invoke command-line main() function even on OS X and Win32.
G3D_START_AT_MAIN();

int main(int argc, const char* argv[]) {
    {
        G3DSpecification g3dSpec;
        g3dSpec.audio = false;
        initGLG3D(g3dSpec);
    }

    GApp::Settings settings(argc, argv);

    // Change the window and other startup parameters by modifying the
    // settings class.  For example:
    settings.window.caption             = argv[0];

    // Set enable to catch more OpenGL errors
    // settings.window.debugContext     = true;

    // Some common resolutions:
    // settings.window.width            =  854; settings.window.height       = 480;
    // settings.window.width            = 1024; settings.window.height       = 768;
    settings.window.width               = 1280; settings.window.height       = 720;
    //settings.window.width             = 1920; settings.window.height       = 1080;
    // settings.window.width            = OSWindow::primaryDisplayWindowSize().x; settings.window.height = OSWindow::primaryDisplayWindowSize().y;
    settings.window.fullScreen          = false;
    settings.window.resizable           = ! settings.window.fullScreen;
    settings.window.framed              = ! settings.window.fullScreen;

    // Set to true for a significant performance boost if your app can't render at 60fps, or if
    // you *want* to render faster than the display.
    settings.window.asynchronous        = false;

    settings.hdrFramebuffer.depthGuardBandThickness = Vector2int16(64, 64);
    settings.hdrFramebuffer.colorGuardBandThickness = Vector2int16(0, 0);
    settings.dataDir                    = FileSystem::currentDirectory();
    settings.screenshotDirectory        = "../journal/";

    settings.renderer.deferredShading = true;
    settings.renderer.orderIndependentTransparency = true;

    return App(settings).run();
}


App::App(const GApp::Settings& settings) : GApp(settings) {
	//Cylinder GUI
	height = 3.0f;
	radius = 2.0f;
	//RayTracer GUI
	fixedPrimitives = false;
	multithreading = true;
	resolutionList.append(GuiText("1x1"));
	resolutionList.append(GuiText("320x200"));
	resolutionList.append(GuiText("640x400"));
	resolutionList.append(GuiText("1024x600"));
	resolutionList.append(GuiText("1440x900"));
	resolutionID = 1;
	raysPerPixel = 512;
	z_near = -3;
	//RayTracer variables
	rayTracer = std::make_unique<RayTracer>();
	sw = std::make_unique<StopWatch>();
	sw->enabled();
	image = nullptr;
	result = nullptr;
}


// Called before the application loop begins.  Load data here and
// not in the constructor so that common exceptions will be
// automatically caught.
void App::onInit() {
    GApp::onInit();
    setFrameDuration(1.0f / 60.0f);
    // Call setScene(shared_ptr<Scene>()) or setScene(MyScene::create()) to replace
    // the default scene here.
    
    showRenderingStats      = false;

    makeGUI();
    // For higher-quality screenshots:
    // developerWindow->videoRecordDialog->setScreenShotFormat("PNG");
    // developerWindow->videoRecordDialog->setCaptureGui(false);

    loadScene(
        //"G3D Sponza"
        //"G3D Cornell Box" // Load something simple
        developerWindow->sceneEditorWindow->selectedSceneName()  // Load the first scene encountered 
        );
}


void App::makeGUI() {
    // Initialize the developer HUD
    createDeveloperHUD();

    debugWindow->setVisible(true);
    developerWindow->videoRecordDialog->setEnabled(true);

	GuiPane* rayPane = debugPane->addPane("Rays", GuiTheme::ORNATE_PANE_STYLE);
	rayPane->setNewChildSize(360);
	rayPane->addDropDownList("Resolution", resolutionList, &resolutionID);
	rayPane->addCheckBox("Use fixed primitives", &fixedPrimitives);
	rayPane->addCheckBox("Multithreading", &multithreading);
	rayPane->addNumberBox("Rays per pixel", &raysPerPixel, "", GuiTheme::LOG_SLIDER, 0, 2048);
	rayPane->addNumberBox("z_near", &z_near, "", GuiTheme::LOG_SLIDER, -10, 10);
	rayPane->beginRow();
		rayPane->addButton("Load Scene", [this]() {
			drawMessage("Loading..."); 
			loadScene(//"G3D Cornell Box" 
				developerWindow->sceneEditorWindow->selectedSceneName());
		});
		rayPane->addButton("Render", [this]() { drawMessage("Rendering..."); render(); });
		rayPane->addButton("Exit", [this]() { m_endProgram = true; });
	rayPane->endRow();
	rayPane->pack();

	/*
	GuiPane* cylinderPane = debugPane->addPane("Cylinder", GuiTheme::ORNATE_PANE_STYLE);
	cylinderPane->addNumberBox("height", &height, "", GuiTheme::LOG_SLIDER, 1.0f, 10.0f);
	cylinderPane->addNumberBox("radius", &radius, "", GuiTheme::LOG_SLIDER, 1.0f, 10.0f);
	cylinderPane->addButton("Generate cylinder", [this]() { makeCylinder(radius, height); drawMessage("Generating Cylinder."); });
	cylinderPane->addButton("Exit", [this]() { m_endProgram = true; });
	cylinderPane->pack();
	*/

    debugWindow->pack();
    debugWindow->setRect(Rect2D::xywh(0, 0, (float)window()->width(), debugWindow->rect().height()));
}


// This default implementation is a direct copy of GApp::onGraphics3D to make it easy
// for you to modify. If you aren't changing the hardware rendering strategy, you can
// delete this override entirely.
void App::onGraphics3D(RenderDevice* rd, Array<shared_ptr<Surface> >& allSurfaces) {
    if (!scene()) {
        if ((submitToDisplayMode() == SubmitToDisplayMode::MAXIMIZE_THROUGHPUT) && (!rd->swapBuffersAutomatically())) {
            swapBuffers();
        }
        rd->clear();
        rd->pushState(); {
            rd->setProjectionAndCameraMatrix(activeCamera()->projection(), activeCamera()->frame());
            drawDebugShapes();
        } rd->popState();
        return;
    }


    GBuffer::Specification gbufferSpec = m_gbufferSpecification;
    extendGBufferSpecification(gbufferSpec);
    m_gbuffer->setSpecification(gbufferSpec);
    m_gbuffer->resize(m_framebuffer->width(), m_framebuffer->height());
    m_gbuffer->prepare(rd, activeCamera(), 0, -(float)previousSimTimeStep(), m_settings.hdrFramebuffer.depthGuardBandThickness, m_settings.hdrFramebuffer.colorGuardBandThickness);

    m_renderer->render(rd, m_framebuffer, scene()->lightingEnvironment().ambientOcclusionSettings.enabled ? m_depthPeelFramebuffer : shared_ptr<Framebuffer>(),
        scene()->lightingEnvironment(), m_gbuffer, allSurfaces);

    // Debug visualizations and post-process effects
    rd->pushState(m_framebuffer); {
        // Call to make the App show the output of debugDraw(...)
        rd->setProjectionAndCameraMatrix(activeCamera()->projection(), activeCamera()->frame());
        drawDebugShapes();
        const shared_ptr<Entity>& selectedEntity = (notNull(developerWindow) && notNull(developerWindow->sceneEditorWindow)) ? developerWindow->sceneEditorWindow->selectedEntity() : shared_ptr<Entity>();
        scene()->visualize(rd, selectedEntity, allSurfaces, sceneVisualizationSettings(), activeCamera());

        // Post-process special effects
        m_depthOfField->apply(rd, m_framebuffer->texture(0), m_framebuffer->texture(Framebuffer::DEPTH), activeCamera(), m_settings.hdrFramebuffer.depthGuardBandThickness - m_settings.hdrFramebuffer.colorGuardBandThickness);

        m_motionBlur->apply(rd, m_framebuffer->texture(0), m_gbuffer->texture(GBuffer::Field::SS_EXPRESSIVE_MOTION),
            m_framebuffer->texture(Framebuffer::DEPTH), activeCamera(),
            m_settings.hdrFramebuffer.depthGuardBandThickness - m_settings.hdrFramebuffer.colorGuardBandThickness);
    } rd->popState();

    // We're about to render to the actual back buffer, so swap the buffers now.
    // This call also allows the screenshot and video recording to capture the
    // previous frame just before it is displayed.
    if (submitToDisplayMode() == SubmitToDisplayMode::MAXIMIZE_THROUGHPUT) {
        swapBuffers();
    }

    // Clear the entire screen (needed even though we'll render over it, since
    // AFR uses clear() to detect that the buffer is not re-used.)
    rd->clear();

    // Perform gamma correction, bloom, and SSAA, and write to the native window frame buffer
    m_film->exposeAndRender(rd, activeCamera()->filmSettings(), m_framebuffer->texture(0), settings().hdrFramebuffer.colorGuardBandThickness.x + settings().hdrFramebuffer.depthGuardBandThickness.x, settings().hdrFramebuffer.depthGuardBandThickness.x);
}


void App::onAI() {
    GApp::onAI();
    // Add non-simulation game logic and AI code here
}


void App::onNetwork() {
    GApp::onNetwork();
    // Poll net messages here
}


void App::onSimulation(RealTime rdt, SimTime sdt, SimTime idt) {
    GApp::onSimulation(rdt, sdt, idt);

    // Example GUI dynamic layout code.  Resize the debugWindow to fill
    // the screen horizontally.
    debugWindow->setRect(Rect2D::xywh(0, 0, (float)window()->width(), debugWindow->rect().height()));
}


bool App::onEvent(const GEvent& event) {
    // Handle super-class events
    if (GApp::onEvent(event)) { return true; }

    // If you need to track individual UI events, manage them here.
    // Return true if you want to prevent other parts of the system
    // from observing this specific event.
    //
    // For example,
    // if ((event.type == GEventType::GUI_ACTION) && (event.gui.control == m_button)) { ... return true; }
    // if ((event.type == GEventType::KEY_DOWN) && (event.key.keysym.sym == GKey::TAB)) { ... return true; }
    // if ((event.type == GEventType::KEY_DOWN) && (event.key.keysym.sym == 'p')) { ... return true; }

    if ((event.type == GEventType::KEY_DOWN) && (event.key.keysym.sym == 'p')) { 
        shared_ptr<DefaultRenderer> r = dynamic_pointer_cast<DefaultRenderer>(m_renderer);
        r->setDeferredShading(! r->deferredShading());
        return true; 
    }

    return false;
}


void App::onUserInput(UserInput* ui) {
    GApp::onUserInput(ui);
    (void)ui;
    // Add key handling here based on the keys currently held or
    // ones that changed in the last frame.
}


void App::onPose(Array<shared_ptr<Surface> >& surface, Array<shared_ptr<Surface2D> >& surface2D) {
    GApp::onPose(surface, surface2D);

    // Append any models to the arrays that you want to later be rendered by onGraphics()
}


void App::onGraphics2D(RenderDevice* rd, Array<shared_ptr<Surface2D> >& posed2D) {
    // Render 2D objects like Widgets.  These do not receive tone mapping or gamma correction.
    Surface2D::sortAndRender(rd, posed2D);
}


void App::onCleanup() {
    // Called after the application loop ends.  Place a majority of cleanup code
    // here instead of in the constructor so that exceptions can be caught.
}

//Setup rendering parameters
void App::render()
{
	std::shared_ptr<Texture> texture;

	resolution = getResolution();
	//Set output image parameters
	image = G3D::Image::create(resolution[0], resolution[1], ImageFormat::RGB8());
	//Pose scene and load surfaces
	G3D::GApp::onPose(surfaces, surfaces2D);
	//Load Raytracer
	rayTracer->useMultiThreading(multithreading);
	rayTracer->useFixedPrimitives(fixedPrimitives);
	rayTracer->loadSurfaces(surfaces);
	rayTracer->loadLights(G3D::GApp::scene());
	rayTracer->loadCamera(G3D::GApp::activeCamera());
	rayTracer->setZNear(z_near);
	rayTracer->setRayPerPixel(raysPerPixel);
	//Raytrace and mesure elapsed time
	sw->tick();
	rayTracer->rayTrace(image);
	sw->tock();
	renderTime = sw->elapsedTime();
	debugPrintf("Render time = %d\n", renderTime);
	G3D::GApp::show(image);
	//Post-process
	texture = G3D::Texture::fromImage("RayTraced Image", image);
	G3D::GApp::m_film->exposeAndRender(renderDevice, m_debugCamera->filmSettings(), texture, settings().hdrFramebuffer.colorGuardBandThickness.x,
		settings().hdrFramebuffer.depthGuardBandThickness.x, result);
	char	buffer[16];
	result->toImage()->save("../output/" + G3D::GApp::scene()->name() + "_result("
		+ G3D::String(itoa((int)resolution.x, buffer, 10)) + "x" + G3D::String(itoa((int)resolution.y, buffer, 10)) + ").png");
	result->toImage()->save("../output/" + G3D::GApp::scene()->name() + "_postprocess("
		+ G3D::String(itoa((int)resolution.x, buffer, 10)) + "x" + G3D::String(itoa((int)resolution.y, buffer, 10)) + ").png");
}

Vector2int32 App::getResolution() const
{
	String s = resolutionList[resolutionID];
	int	x = 1;
	int	y = 1;

	sscanf(s.c_str(), "%dx%d", &x, &y);
	return Vector2int32(x, y);
}
