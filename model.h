#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "SOIL2/SOIL2.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"

using namespace std;

GLint TextureFromFile(const char* path, string directory);


class Model
{


public:

    // constructor - load all models linked by paths
    Model(vector<string> paths)
    {
        this->loadModel(paths);
    }

    // draw each mesh within the model class using shader program
    //void Draw(Shader shader)
    void Draw(ShaderProgram* shader)
    {
        for (GLuint i = 0; i < this->meshes.size(); i++)
        {
            this->meshes[i].Draw(shader);
        }
    }

    void openLid(GLuint ID, GLfloat angle)
    {
        cout << "Model::openLid \n";
    }

    void closeLid(GLuint ID, GLfloat angle)
    {
        cout << "Model::closeLid\n";
    }

    void rotateMesh(int meshID, glm::vec3 rotation)
    {
        if (meshID >= 0 && meshID < meshes.size())
        {
            cout << "Model::rorateMesh\n";
            cout << "\tRequested rotation:\n";
            cout << "\t\tx: " << rotation.x << "\ty: " << rotation.y << "\tz: " << rotation.z << endl;

            this->meshes[meshID].rotate(rotation);
        }
        else {
            cout << "Wrong mesh ID\n";
        }
    }

    void moveMesh(int meshID, glm::vec3 translation)
    {
        if (meshID >= 0 && meshID < meshes.size())
        {
            cout << "Model::moveMesh\n";
            cout << "\tRequested translation:\n";
            cout << "\t\tx: " << translation.x << "\ty: " << translation.y << "\tz: " << translation.z << endl;

            this->meshes[meshID].move(translation);
        }
        else {
            cout << "Wrong mesh ID\n";
        }
    }
       
    // called when a piano key is pressed
    void keyPressed(int keyNum)
    {
        cout << "Model::keyPressed(" << keyNum << ")\n";  

        int elementsInKey = 8;  // the number of meshes making up 1 piano key

        // save pointers to each of the elements making up the pressed key
        // (elements are saved in [meshes] one after another)
        // the 3 immobile elements of the key are ignored
        Mesh* base = &meshes[(keyNum - 1) * elementsInKey];
        Mesh* hammer = &meshes[(keyNum - 1) * elementsInKey + 1];
        Mesh* wippen = &meshes[(keyNum - 1) * elementsInKey + 2];
        Mesh* repetition_lever = &meshes[(keyNum - 1) * elementsInKey + 3];
        Mesh* jack = &meshes[(keyNum - 1) * elementsInKey + 4];

  
        // set all the mobile elements of the pressed key in motion
        for (int keyElem = 0; keyElem < 5; keyElem++)
        {
            this->meshes[(keyNum - 1) * elementsInKey + keyElem].isFalling = false;
            this->meshes[(keyNum - 1) * elementsInKey + keyElem].isRising = true;
        }
    }

    // called when a piano key is released
    void keyReleased(int keyNum)
    {
        cout << "Model::keyReleased("<<keyNum<<")\n";

        int elementsInKey = 8;  // the number of meshes making up 1 piano key

        // save pointers to each of the elements making up the pressed key
        // (elements are saved in [meshes] one after another)
        // the 3 immobile elements of the key are ignored
        Mesh* base = &meshes[(keyNum - 1) * elementsInKey];
        Mesh* hammer = &meshes[(keyNum - 1) * elementsInKey + 1];
        Mesh* wippen = &meshes[(keyNum - 1) * elementsInKey + 2];
        Mesh* repetition_lever = &meshes[(keyNum - 1) * elementsInKey + 3];
        Mesh* jack = &meshes[(keyNum - 1) * elementsInKey + 4];

        // set all the mobile elements of the pressed key in motion
        for (int keyElem = 0; keyElem < 5; keyElem++)
        {
            this->meshes[(keyNum - 1) * elementsInKey + keyElem].isRising = false;
            this->meshes[(keyNum - 1) * elementsInKey + keyElem].isFalling = true;
        }
    }



private:

    vector<Mesh> elements;  // stores all mesh prototypes
    vector<Mesh> meshes;    // stores all loaded meshes
    string directory;       // ???
    vector<Texture> textures_loaded;	// stores all loaded textures

    // debugging: print the name of each loaded mesh
    void checkMeshes()
    {
        cout << "Contents of <meshes>:\n";
        for (int i = 0; i < meshes.size(); i++)
        {
            cout << i << ": " << meshes[i].getName() << endl;
        }
    }

    // Set the position vector for the prototype elements
    // (prototypes are rendered with their origin point at (0, 0, 0) - for easier origin-based rotation)
    void updateElementPositions()
    {
        this->elements[0].setPosition(glm::vec3(-0.816625f, 0.794821f, 0.865183f)); // base black
        this->elements[1].setPosition(glm::vec3(-0.816625f, 0.794821f, 0.865183f)); // base white01
        this->elements[2].setPosition(glm::vec3(-0.816625f, 0.794821f, 0.865183f)); // base white02
        this->elements[3].setPosition(glm::vec3(-0.816625f, 0.794821f, 0.865183f)); // base white03
        this->elements[4].setPosition(glm::vec3(-0.816625f, 0.926671f, 0.779144f)); // hammer
        this->elements[5].setPosition(glm::vec3(-0.816625f, 0.843621f, 0.66461f));  // wippen
        this->elements[6].setPosition(glm::vec3(-0.816625f, 0.882167f, 0.72373f));  // repetition lever
        this->elements[7].setPosition(glm::vec3(-0.816625f, 0.846238f, 0.778807f)); // jack 
        this->elements[8].setPosition(glm::vec3(-0.816625f, 0.924906f, 0.797623f)); // top bar
        this->elements[9].setPosition(glm::vec3(-0.816625f, 0.872364f, 0.809503f)); // cylinder
        this->elements[10].setPosition(glm::vec3(-0.816625f, 0.843621f, 0.66461f)); // bottom holder
    }

    // Set Rotation limits for the mobile parts of the key
    // (since there's no collision detection, the elements only rotate to a certain set degree untill they either stop or start falling)
    void setElementRotationLimits()
    {
        if (this->elements.size() >= 8) // 8 is the number of the rotating prototype elements
        {
            this->elements[0].setRotationLimit(1.88f);  // black base
            this->elements[1].setRotationLimit(1.88f);  // white base 1
            this->elements[2].setRotationLimit(1.88f);  // white base 2
            this->elements[3].setRotationLimit(1.88f);  // white base 3
            this->elements[4].setRotationLimit(20.9f);  // hammer
            this->elements[5].setRotationLimit(-3.04f); // wippen
            this->elements[6].setRotationLimit(2.5f);   // repetition_lever
            this->elements[7].setRotationLimit(11.6f);  // jack
           
        }
        else {
            cout << "Model::setElementRotationLimits: elements not loaded properly\n";
        }
    }

    // set the names for each prototype element (for easier mesh loading debugging)
    void setElementNames() 
    {
        if (this->elements.size() >= 11) // 11 is the number of all prototype elements in a key
        {
            this->elements[0].setName("key_base_black");
            this->elements[1].setName("key_base_white01");
            this->elements[2].setName("key_base_white02");
            this->elements[3].setName("key_base_white03");
            this->elements[4].setName("key_hammer");
            this->elements[5].setName("key_wippen");
            this->elements[6].setName("key_repetition_lever");
            this->elements[7].setName("key_jack");
            this->elements[8].setName("key_top_bar");
            this->elements[9].setName("key_jack_cylinder");
            this->elements[10].setName("key_bottom_handle");
        }
        else {
            cout << "Model::setElementRotationLimits: elements not loaded properly\n";
        }
    }
    
    // set the parent element for the elements rotating around their parent origin
    void setMeshParents() {
        int elementsInKey = 8;
        int numOfKeyMeshes = 87 * elementsInKey; // 87 keys in a full keyboard
        if (meshes.size() >= numOfKeyMeshes)
        {
            for (int i = 0; i < 87; i++)
            {
                this->meshes[i * elementsInKey + 4].setParent(&this->meshes[i * elementsInKey + 2]); // jack -> wippen
                this->meshes[i * elementsInKey + 3].setParent(&this->meshes[i * elementsInKey + 2]); // repetition lever -> wippen

            }
        }
        else {
            cout << "Model::setMeshParents: meshes not loaded properly\n";
        }
    }
    

    // load the 12-key-long repeatable pattern and set each key in the right position
    // (new position calculated as an offset on the x axis from the starting position)
    void addRepeatableKeys()
    {
        // distances between different kinds of keys
        GLfloat d1 = 0.020929f, d2 = 0.023909f, d3 = 0.016191f;
        
        // draw the first 12 pattern keys 
        // (the same sequence of keys will be repeated later along the x axis to render the entire keyboard)

        // 1 (white)
        this->meshes.push_back(elements[1]); // elements[1]: white key base (1)
        this->meshes.back().move(glm::vec3(2*d1 + d2, 0.0f, 0.0f));
        for (int j = 4; j <= 10; j++) // elements[4-9]: hammer, wippen, repetition_lever, jack, top_bar, jack_cylinder
        {
            this->meshes.push_back(elements[j]);
            this->meshes.back().move(glm::vec3(2 * d1 + d2, 0.0f, 0.0f));
        }

        // 2 (black)
        this->meshes.push_back(elements[0]); 
        this->meshes.back().move(glm::vec3(3 * d1 + d2, 0.0f, 0.0f));
        for (int j = 4; j <= 10; j++)
        {
            this->meshes.push_back(elements[j]);
            this->meshes.back().move(glm::vec3(3 * d1 + d2, 0.0f, 0.0f));
        }

        // 3 (white)
        this->meshes.push_back(elements[3]);
        this->meshes.back().move(glm::vec3(3 * d1 + d2 + d3, 0.0f, 0.0f));
        for (int j = 4; j <= 10; j++)
        {
            this->meshes.push_back(elements[j]);
            this->meshes.back().move(glm::vec3(3 * d1 + d2 + d3, 0.0f, 0.0f));
        }

        // 4 (black)
        this->meshes.push_back(elements[0]);
        this->meshes.back().move(glm::vec3(3 * d1 + d2 + 2 * d3, 0.0f, 0.0f));
        for (int j = 4; j <= 10; j++)
        {
            this->meshes.push_back(elements[j]);
            this->meshes.back().move(glm::vec3(3 * d1 + d2 + 2 * d3, 0.0f, 0.0f));
        }

        // 5 (white)
        this->meshes.push_back(elements[2]);
        this->meshes.back().move(glm::vec3(4 * d1 + d2 + 2 * d3, 0.0f, 0.0f));
        for (int j = 4; j <= 10; j++)
        {
            this->meshes.push_back(elements[j]);
            this->meshes.back().move(glm::vec3(4 * d1 + d2 + 2 * d3, 0.0f, 0.0f));
        }

        // 6 (white)
        this->meshes.push_back(elements[1]);
        this->meshes.back().move(glm::vec3(4 * d1 + 2 * d2 + 2 * d3, 0.0f, 0.0f));
        for (int j = 4; j <= 10; j++)
        {
            this->meshes.push_back(elements[j]);
            this->meshes.back().move(glm::vec3(4 * d1 + 2 * d2 + 2 * d3, 0.0f, 0.0f));
        }

        // 7 (black)
        this->meshes.push_back(elements[0]);
        this->meshes.back().move(glm::vec3(5 * d1 + 2 * d2 + 2 * d3, 0.0f, 0.0f));
        for (int j = 4; j <= 10; j++)
        {
            this->meshes.push_back(elements[j]);
            this->meshes.back().move(glm::vec3(5 * d1 + 2 * d2 + 2 * d3, 0.0f, 0.0f));
        }

        // 8 (white)
        this->meshes.push_back(elements[3]);
        this->meshes.back().move(glm::vec3(5 * d1 + 2 * d2 + 3 * d3, 0.0f, 0.0f));
        for (int j = 4; j <= 10; j++)
        {
            this->meshes.push_back(elements[j]);
            this->meshes.back().move(glm::vec3(5 * d1 + 2 * d2 + 3 * d3, 0.0f, 0.0f));
        }

        // 9 (black)
        this->meshes.push_back(elements[0]);
        this->meshes.back().move(glm::vec3(5 * d1 + 2 * d2 + 4 * d3, 0.0f, 0.0f));
        for (int j = 4; j <= 10; j++)
        {
            this->meshes.push_back(elements[j]);
            this->meshes.back().move(glm::vec3(5 * d1 + 2 * d2 + 4 * d3, 0.0f, 0.0f));
        }

        // 10 (white)
        this->meshes.push_back(elements[3]);
        this->meshes.back().move(glm::vec3(5 * d1 + 2 * d2 + 5 * d3, 0.0f, 0.0f));
        for (int j = 4; j <= 10; j++)
        {
            this->meshes.push_back(elements[j]);
            this->meshes.back().move(glm::vec3(5 * d1 + 2 * d2 + 5 * d3, 0.0f, 0.0f));
        }

        // 11 (black)
        this->meshes.push_back(elements[0]);
        this->meshes.back().move(glm::vec3(5 * d1 + 2 * d2 + 6 * d3, 0.0f, 0.0f));
        for (int j = 4; j <= 10; j++)
        {
            this->meshes.push_back(elements[j]);
            this->meshes.back().move(glm::vec3(5 * d1 + 2 * d2 + 6 * d3, 0.0f, 0.0f));
        }

        // 12 (white)
        this->meshes.push_back(elements[2]);
        this->meshes.back().move(glm::vec3(6 * d1 + 2 * d2 + 6 * d3, 0.0f, 0.0f));
        for (int j = 4; j <= 10; j++)
        {
            this->meshes.push_back(elements[j]);
            this->meshes.back().move(glm::vec3(6 * d1 + 2 * d2 + 6 * d3, 0.0f, 0.0f));
        }

        cout << "Model::addRepeatableKeys: Loaded " << meshes.size() << " meshes (all the repeatable meshes).\n";

    }

    // load the entire keyboard of keys by repeating the key pattern and translating it on the x axis by a set increment
    void addKeys()
    {   
        // distance between each key
        GLfloat increment = 0.23f, d1 = 0.020929f, d2= 0.023909f, d3 = 0.016191f;

        // add the first 3 off-pattern keys
        // 1 (white)
        this->meshes.push_back(elements[1]);
        for (int i = 4; i <= 10; i++)
        {
            this->meshes.push_back(elements[i]);
        }

        // 2 (black)
        this->meshes.push_back(elements[0]);
        this->meshes.back().move(glm::vec3(d1, 0.0f, 0.0f));
        for (int i = 4; i <= 10; i++)
        {
            this->meshes.push_back(elements[i]);
            this->meshes.back().move(glm::vec3(d1, 0.0f, 0.0f));
        }   

        // 3 (white)
        this->meshes.push_back(elements[2]);
        this->meshes.back().move(glm::vec3(2 * d1, 0.0f, 0.0f));
        for (int i = 4; i <= 10; i++)
        {
            this->meshes.push_back(elements[i]);
            this->meshes.back().move(glm::vec3(2 * d1, 0.0f, 0.0f));
        }
        
        // add the repeatable key pattern 7 times for a full keyboard
        for (int i = 0; i < 7; i++)
        { 
            addRepeatableKeys();

            // move each pattern group (of 12 keys - each consisting of 8 meshes!) by a set increment
            for (int j = 12 * 8; j > 0; j--) 
            {
                this->meshes[meshes.size() - j].move(GLfloat(i) * glm::vec3(increment, 0.0f, 0.0f));
            }
        }

    }


    // Loads a model with supported ASSIMP extensions from an .obj file and stores the resulting meshes in the <meshes> vector.
    void loadModel(vector<string> paths)
    {
        cout << "model::loadModel() initiated\n";

        // Define an ASSIMP importer object
        Assimp::Importer importer;

        // store all assimp-generated scenes in a vector (always 1 scene generated per 1 file import)
        vector<const aiScene*> scenes;

        // import all elements from <paths> into a new assimp scene 
        for (int i = 0; i < paths.size(); i++)
        {
            scenes.push_back(importer.ReadFile(paths[i], aiProcess_Triangulate | aiProcess_FlipUVs));

            // Check for errors
            if (!scenes[i] || scenes[i]->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scenes[i]->mRootNode)
            {
                cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
                return;
            }
            
            // Retrieve the directory path of the filepath
            this->directory = paths[i].substr(0, paths[i].find_last_of('/'));

            // Process ASSIMP's root node recursively
            this->processNode(scenes[i]->mRootNode, scenes[i]);
        }

        // after loading all prototype elements - do all the neccessary updates
        updateElementPositions();
        setElementRotationLimits();
        setElementNames();

        // spawn key prototypes
        addKeys();

        // set the parent mehses within each key across the full keyboard
        setMeshParents();

        // check the order of the loaded meshes
        checkMeshes();
         
    }

    // ... after importing an .obj file into an ASSIMP scene:
    // Proccess the ASSIMP nodes within the .obj file recursively
    void processNode(aiNode* node, const aiScene* scene)
    {
        cout << "Model::processNode()\n";

        // Process each mesh located in the current node
        for (GLuint i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

            // save the proccessed meshes into the prototype <elements> vector
            this->elements.push_back(this->processMesh(mesh, scene));
        }

        // proccess the children nodes (if there are any)
        for (GLuint i = 0; i < node->mNumChildren; i++)
        {
            this->processNode(node->mChildren[i], scene);
        }
    }

    // retrieve information about the vertices, indices and textures of the loaded mesh
    Mesh processMesh(aiMesh* mesh, const aiScene* scene)
    {
        cout << "Model::processMesh()\n";

        vector<Vertex> vertices;
        vector<GLuint> indices;
        vector<Texture> textures;

        cout << "Mesh vertices number: " << mesh->mNumVertices << endl;
        cout << "Mesh has normals?: " << mesh->HasNormals() << endl;

        // process the vertices in a mesh
        for (GLuint i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            glm::vec3 vector; // placeholder vector for ASSIMP -> glm type conversion

            // positions
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;

            // normals
            if (mesh->HasNormals()) {
                vector.x = mesh->mNormals[i].x;
                vector.y = mesh->mNormals[i].y;
                vector.z = mesh->mNormals[i].z;
                vertex.Normal = vector;
            }

            // texture coordinates
            if (mesh->mTextureCoords[0]) // check if mesh has texCoords
            {
                glm::vec2 vec;
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
            }
            else
            {
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
            }

            // save the proccessed vertex
            vertices.push_back(vertex);
        }

        // process the indices of each face (triangle)
        for (GLuint i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (GLuint j = 0; j < face.mNumIndices; j++)
            {
                indices.push_back(face.mIndices[j]);
            }
        }

        // Process materials (textures)
        if (mesh->mMaterialIndex >= 0)
        {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            // We assume a convention for sampler names in the shaders. Each diffuse texture should be named
            // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
            // Same applies to other texture as the following list summarizes:
            // Diffuse: texture_diffuseN
            // Specular: texture_specularN
            // Normal: texture_normalN

            // 1. Diffuse maps
            vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

            // 2. Specular maps
            vector<Texture> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

            // 3. Normal maps?
            vector<Texture> normalMaps = this->loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        }

        // Return a mesh constructor using the retrieved data
        return Mesh(vertices, indices, textures);
    }


    // TUTAJ SKOÑCZY£AM U£ADNIANIE KODU XD  //  




    // Checks all material textures of a given type and loads the textures if they're not loaded yet.
    // The required info is returned as a Texture struct.
    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
    {
        vector<Texture> textures;

        for (GLuint i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);

            // Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
            GLboolean skip = false;

            for (GLuint j = 0; j < textures_loaded.size(); j++)
            {
                if (textures_loaded[j].path == str)
                {
                    textures.push_back(textures_loaded[j]);
                    skip = true; // A texture with the same filepath has already been loaded, continue to next one. (optimization)

                    break;
                }
            }

            if (!skip)
            {   // If texture hasn't been loaded already, load it
                Texture texture;
                texture.id = TextureFromFile(str.C_Str(), this->directory);
                texture.type = typeName;
                texture.path = str;
                textures.push_back(texture);

                this->textures_loaded.push_back(texture);  // Store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
            }
        }

        return textures;
    }
};

GLint TextureFromFile(const char* path, string directory)
{
    //Generate texture ID and load texture data
    string filename = string(path);
    filename = directory + '/' + filename;
    GLuint textureID;
    glGenTextures(1, &textureID);

    int width, height;

    unsigned char* image = SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_RGB);

    // Assign texture to ID
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    SOIL_free_image_data(image);

    return textureID;
}