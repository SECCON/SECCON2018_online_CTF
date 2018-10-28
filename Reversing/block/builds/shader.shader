Shader "Custom/shader" {
	Properties {
		_MainTex ("Albedo", 2D) = "white" {}
	}
	SubShader {
		Tags { "RenderType"="Opaque" }
		LOD 200

		CGPROGRAM
		// Physically based Standard lighting model, and enable shadows on all light types
		#pragma surface surf Standard fullforwardshadows

		// Use shader model 3.0 target, to get nicer looking lighting
		#pragma target 3.0

		sampler2D _MainTex;

		struct Input {
			float2 uv_MainTex;
		};

		void surf (Input IN, inout SurfaceOutputStandard o) {
			// Albedo comes from a texture tinted by color
            fixed4 c;
            if (IN.uv_MainTex.x >= 0.375 && IN.uv_MainTex.x < 0.422) {
                IN.uv_MainTex.x += 0.050;
            } else if (IN.uv_MainTex.x >= 0.422 && IN.uv_MainTex.x < 0.469) {
                IN.uv_MainTex.x -= 0.050;
            } else if (IN.uv_MainTex.x >= 0.180 && IN.uv_MainTex.x < 0.253) {
                IN.uv_MainTex.x += 0.1;
            } else if (IN.uv_MainTex.x >= 0.276 && IN.uv_MainTex.x < 0.348) {
                IN.uv_MainTex.x -= 0.095;
            } else {
            }
            c = tex2D (_MainTex, IN.uv_MainTex);
			o.Albedo = c.rgb;
		}
		ENDCG
	}
	FallBack "Diffuse"
}
