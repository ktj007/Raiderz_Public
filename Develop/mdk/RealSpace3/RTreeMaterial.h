#pragma once

namespace rs3 {

class RS_API RTreeMaterial
{
public:
        RTreeMaterial( )
        {
            m_afAmbient[0] = m_afAmbient[1] = m_afAmbient[2] = m_afAmbient[3] = 1.0f;
            m_afDiffuse[0] = m_afDiffuse[1] = m_afDiffuse[2] = m_afDiffuse[3] = 1.0f;
            m_afSpecular[0] = m_afSpecular[1] = m_afSpecular[2] = m_afSpecular[3] = 1.0f;
            m_afEmission[0] = m_afEmission[1] = m_afEmission[2] = m_afEmission[3] = 1.0f;
            m_fShininess = 5.0f;
        }

        void Set(const float* pMaterialArray)
        {
            // assumes CSpeedTreeRT-format data packing
            memcpy(m_afDiffuse, pMaterialArray, 3 * sizeof(float));
            memcpy(m_afAmbient, pMaterialArray + 3, 3 * sizeof(float));

            memcpy(m_afSpecular, pMaterialArray + 6, 3 * sizeof(float));
            memcpy(m_afEmission, pMaterialArray + 9, 3 * sizeof(float));
            m_fShininess = pMaterialArray[12];
        }

        void SetAmbient(const float* pAmbient)
        {
            memcpy(m_afAmbient, pAmbient, 3 * sizeof(float));
        }

        void ScaleAmbient(float fScalar)
        {
            m_afAmbient[0] *= fScalar;
            m_afAmbient[1] *= fScalar;
            m_afAmbient[2] *= fScalar;
        }

        const float*    Ambient(void) const     { return m_afAmbient; }
        const float*    Diffuse(void) const     { return m_afDiffuse; }
        const float*    Specular(void) const    { return m_afSpecular; }
        const float*    Emission(void) const    { return m_afEmission; }
        float           Shininess(void) const   { return m_fShininess; }

		void			ApplyToDevice(RDevice* device);

private:
        float           m_afAmbient[4];         // ambient color (r, g, b, a) [each range from 0.0 to 1.0]
        float           m_afDiffuse[4];         // diffuse color (r, g, b, a) [each range from 0.0 to 1.0]
        float           m_afSpecular[4];        // specular color (r, g, b, a) [each range from 0.0 to 1.0]
        float           m_afEmission[4];        // emission color (r, g, b, a) [each range from 0.0 to 1.0]
        float           m_fShininess;           // shininess [ranges from 0.0 to 128.0]
};

}
