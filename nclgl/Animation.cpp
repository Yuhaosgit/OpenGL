//#include "Animation.h"
//#include "../nclgl/OGLRenderer.h"
//
//Animation::Animation(const std::string& FileName) {
//	objectAnime = new MeshAnimation(FileName);
//	if (objectAnime->GetFrameCount() == 0) {
//		throw FileName + "does not contain animation information";
//	}
//}
//
//Animation::Animation(const Animation& copy) {
//	currentFrame = 0;
//	frameTime = 0.0;
//
//	objectAnime = copy.objectAnime;
//}
//
//void Animation::operator=(const Animation& copy) {
//	currentFrame = 0;
//	frameTime = 0.0;
//
//	objectAnime = copy.objectAnime;
//}

//void Animation::RenderAnimation(Shader* shader, Mesh* mesh) {
//	vector<Matrix4> frame_matrices;
//
//	const Matrix4* inv_bind_pose = mesh->GetInverseBindPose();
//	const Matrix4* frame_data = objectAnime->GetJointData(currentFrame);
//
//	for (int i = 0; i < mesh->GetJointCount(); ++i)
//		frame_matrices.emplace_back(frame_data[i] * inv_bind_pose[i]);
//
//	glUniformMatrix4fv
//	(glGetUniformLocation(shader->GetProgram(), "joints")
//		, frame_matrices.size(), false, (float*)frame_matrices.data());
//}

//void Animation::Update(const float& dt) {
//	if (objectAnime != nullptr) {
//		frameTime -= dt;
//		while (frameTime < 0.0f) {
//			currentFrame = (currentFrame + 1) % objectAnime->GetFrameCount();
//			frameTime += 1.0f / objectAnime->GetFrameRate();
//		}
//	}
//}