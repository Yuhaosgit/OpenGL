#pragma once
#include "FrameBuffer.h"
#include "RenderPass.h"
#include "Camera.h"
#include "Importer.h"
class Pipeline {
public:
	Pipeline(std::shared_ptr<Camera> in_camera) :camera(in_camera){}
	virtual void Render() = 0;
	std::weak_ptr<Camera> camera;
};

class DeferredPipeline :public Pipeline{
public:
	DeferredPipeline(std::shared_ptr<Camera> in_camera) :Pipeline(in_camera) {
		GeometryBuffer = std::make_unique<FrameBuffer>();
		auto colorRoughnessBuffer = std::make_shared<RenderTexture>
			(camera.lock()->width, camera.lock()->height, Dimension::Texture2D, RenderTextureFormat::RGBA8);
		auto normalMetallicBuffer = std::make_shared<RenderTexture>
			(camera.lock()->width, camera.lock()->height, Dimension::Texture2D, RenderTextureFormat::RGBA8);
		auto stencilDepthBuffer = std::make_shared<RenderTexture>
			(camera.lock()->width, camera.lock()->height, Dimension::Texture2D, RenderTextureFormat::STENCIL_DEPTH);
		GeometryBuffer->SetRenderTexture("colorRoughnessBuffer", colorRoughnessBuffer);
		GeometryBuffer->SetRenderTexture("normalMetallicBuffer", normalMetallicBuffer);
		GeometryBuffer->SetRenderTexture("stencilDepthBuffer", stencilDepthBuffer);

		LightBuffer = std::make_unique<FrameBuffer>();
		auto lightedBuffer = std::make_shared<RenderTexture>
			(camera.lock()->width, camera.lock()->height, Dimension::Texture2D, RenderTextureFormat::RGBA8);
		auto lightStencilDepthBuffer = std::make_shared<RenderTexture>
			(camera.lock()->width, camera.lock()->height, Dimension::Texture2D, RenderTextureFormat::STENCIL_DEPTH);
		LightBuffer->SetRenderTexture("lightedBuffer", lightedBuffer);
		LightBuffer->SetRenderTexture("stencilDepthBuffer", lightStencilDepthBuffer);
	}

	std::unique_ptr<FrameBuffer> GeometryBuffer;
	std::unique_ptr<FrameBuffer> LightBuffer;

	virtual void Render() override {
		RenderPass<ShadowPass>::Pass(camera.lock().get());

		GeometryBuffer->BindFrameBuffer();
		RenderPass<GbufferPass>::Pass(camera.lock().get());

		LightBuffer->BindFrameBuffer();
		FrameBuffer::BlitStencil(GeometryBuffer->GetRenderTexture("stencilDepthBuffer"),
			LightBuffer->GetRenderTexture("stencilDepthBuffer"));
		RenderPass<DirectLightPass>::Instance()->colorRoughnessBuffer = GeometryBuffer->GetRenderTexture("colorRoughnessBuffer");
		RenderPass<DirectLightPass>::Instance()->normalMetallicBuffer = GeometryBuffer->GetRenderTexture("normalMetallicBuffer");
		RenderPass<DirectLightPass>::Instance()->stencilDepthBuffer = GeometryBuffer->GetRenderTexture("stencilDepthBuffer");
		RenderPass<DirectLightPass>::Pass(camera.lock().get());

		FrameBuffer::BindScreenFrameBuffer();
		RenderPass<CombinePass>::Instance()->opaqueTex = LightBuffer->GetRenderTexture("lightedBuffer");
		RenderPass<CombinePass>::Instance()->depthTex = GeometryBuffer->GetRenderTexture("stencilDepthBuffer");
		RenderPass<CombinePass>::Pass(camera.lock().get());
	}
};