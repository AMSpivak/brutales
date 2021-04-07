#include "gl_model.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include "../gl_engine/glm/glm.hpp"
#include "../gl_engine/glresourses.h"



/*void glModel::LoadModelBones(std::string FileName)
{
	IGlJubStruct * bone_ptr = jub_bones.get();
	LoadBonesArray(FileName,bone_ptr->bones,bone_ptr->bonescount);

}*/

void glModel::Draw()
{
	
	if(jal_mesh->vertexcount > 2)
	{
	    glBindVertexArray(jal_mesh->VAO);
	    glDrawArrays(GL_TRIANGLES, 0, jal_mesh->vertexcount);
		//scene.jal_mesh = jal_mesh;
	}
}

void glModel::Draw(GlScene::Scene& scene)
{

	if (jal_mesh->vertexcount > 2)
	{
		if(scene.jal_mesh.expired() || (scene.jal_mesh.lock() != jal_mesh))
		{
			glBindVertexArray(jal_mesh->VAO);
		}
		glDrawArrays(GL_TRIANGLES, 0, jal_mesh->vertexcount);
		scene.jal_mesh = jal_mesh;
	}
}

void glModel::SetDrawMatrix(const glm::mat4 &value)
{
	draw_matrix = value;
}
void glModel::Draw(GlScene::Scene &scene, Animation &animation, int now_frame)
{
	Draw(scene, animation,now_frame,draw_matrix);
}
void glModel::Draw(GlScene::Scene& scene, Animation& animation, int now_frame, const glm::mat4& matrix)
{
	if(!scene.LockedShader)
	{
		if (m_shader && (scene.render_shader != m_shader))
		{

			scene.render_shader = m_shader;
			glUseProgram(scene.render_shader);
			unsigned int cameraLoc = glGetUniformLocation(scene.render_shader, "camera");
			glUniformMatrix4fv(cameraLoc, 1, GL_FALSE, glm::value_ptr(scene.render_camera->CameraMatrix()));

			m_material->Assign(scene.render_shader, 0, 0, "NormalTexture", "UtilityTexture");
			scene.material = m_material;
		}
		else
		{
			if (auto material = scene.material.lock())
			{
				if (!(*m_material == *material))
				{
					m_material->Assign(scene.render_shader, 0, 0, "NormalTexture", "UtilityTexture");
					scene.material = m_material;
				}
			}
			else
			{
				m_material->Assign(scene.render_shader, 0, 0, "NormalTexture", "UtilityTexture");
				scene.material = m_material;
			}
		}
	}
	
	unsigned int modelLoc = glGetUniformLocation(scene.render_shader, "model");
	unsigned int drawLoc = glGetUniformLocation(scene.render_shader, "draw");
	unsigned int boneLoc  = glGetUniformLocation(scene.render_shader, "u_BoneMatrices");
	unsigned int color_loc = glGetUniformLocation(scene.render_shader, "color_corrector");
	unsigned int rough_metalLoc = glGetUniformLocation(scene.render_shader, "rzrv_rough_metal_corrector");
	//glUniform4fv(color_loc, 1, glm::value_ptr(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)));
	//glUniform4fv(color_loc, 1, glm::value_ptr(glm::vec4(1.0f,0.88f,0.608f,1.0f)));
	//glUniform3fv(rough_metalLoc, 1, glm::value_ptr(glm::vec3( 1.0f, 0.99f, 1.0f)));

	glUniform4fv(color_loc, 1, glm::value_ptr(m_material->m_color));
	glUniform4fv(rough_metalLoc, 1, glm::value_ptr(m_material->m_rough_metal));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(drawLoc, 1, GL_FALSE, glm::value_ptr(matrix));

	const std::vector <Bone> &bones = jub_bones->bones;

	
	glUniformMatrix4fv(boneLoc, bones.size(), GL_FALSE, animation.GetDrawValues(now_frame,bones));
	
	Draw(scene);
}

void glModel::Draw(GlScene::Scene &scene, int now_frame)
{
	Draw(scene, *animation ,m_frame);
}
void glModel::Draw(GlScene::Scene &scene, int now_frame,const glm::mat4 &matrix)
{
	Draw(scene, *animation ,now_frame,matrix);
}
void glModel::AttachAnimation(std::vector <std::shared_ptr<Animation> > &animations, std::string Filename)
{
	animations.emplace_back(std::make_shared<Animation>());
	animations.back()->LoadAnimation(Filename);
	animation = animations.back();
}
	
const glm::mat4 &glModel::GetBoneMatrix(size_t frame, size_t bone)
{
	return animation->GetBoneMatrix(frame,bone,jub_bones->bones);//frames[now_frame].bones[Models[i]->parent_bone];
}

const glm::mat4 &glModel::GetRotationMatrix(size_t frame)
{
	return animation->GetRotationMatrix(frame,jub_bones->bones);//frames[now_frame].bones[Models[i]->parent_bone];
}

void glModel::LoadAll(std::string FileName)
{
	size_t pos = FileName.rfind("/");
	std::ifstream modelfile;
	modelfile.open(FileName);

	std::string path = pos == std::string::npos ? "" : FileName.erase(pos+1,std::string::npos);
	std::string tmp_str;
	std::string jal_name;
	std::string jub_name;
	std::string png_name;
    std::string png_utility_name;
	std::string frames_name = "";

	getline(modelfile, tmp_str);
	jal_name = /*path + */tmp_str;
	getline(modelfile, tmp_str);
	jub_name = path + tmp_str;
	getline(modelfile, tmp_str);
	png_name = /*path + */tmp_str;
    getline(modelfile, tmp_str);
    png_utility_name = /*path + */tmp_str;
	getline(modelfile, tmp_str);
    std::string png_normal_name = /*path + */tmp_str;
	//std::cout<<"\n===\n";
	modelfile >> parent_idx >> parent_bone >> frames_name;
    //std::cout<<jal_name<<"\n"<<jub_name<<"\n"<<png_name<<"\n"<<png_utility_name<<"\n"<<"!"<<parent_idx<<"!"<<parent_bone<<"\n"<<frames_name<<"\n";
	getline(modelfile, tmp_str);

	getline(modelfile, tmp_str);
	if(tmp_str == "")
	{
		tmp_str = "deff_1st_pass";
	}
	m_shader = GetResourceManager()->GetShader(tmp_str);
	


	GLResourcesManager * resources = GetResourceManager();

	jal_mesh = resources->m_mesh_atlas.Assign(jal_name);


	m_material = std::make_shared<GameResource::GlMaterial>(png_name,png_normal_name,png_utility_name);

	tmp_str = "";
	getline(modelfile, tmp_str);
	if (tmp_str == "")
	{
		m_material->m_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	}
	else
	{
		std::stringstream sstream;
		sstream << tmp_str;
		sstream >> m_material->m_color;
		m_material->m_color = glm::pow(m_material->m_color,glm::vec4(2.2f));
	}
	tmp_str = "";
	getline(modelfile, tmp_str);
	if (tmp_str == "")
	{
		m_material->m_rough_metal = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	}
	else
	{
		std::stringstream sstream;
		sstream << tmp_str;
		sstream >> m_material->m_rough_metal;
	}
	/**/

	modelfile.close();

	jub_bones = resources->m_bones_atlas.Assign(jub_name);
	if(frames_name.compare("")) animation = resources->m_animation_atlas.Assign(frames_name);
}