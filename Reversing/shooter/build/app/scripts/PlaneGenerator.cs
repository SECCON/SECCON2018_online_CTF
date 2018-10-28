using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlaneGenerator : MonoBehaviour {
    public GameObject planePrefab;
    public float span = 1.0f;
    float delta = 0;

	// Use this for initialization
	void Start () {
		
	}
	
	// Update is called once per frame
	void Update () {
        this.delta += Time.deltaTime;
        if (this.delta >= this.span)
        {
            this.delta = 0;
            this.span *= 0.97f;
            Instantiate(planePrefab);
        }
	}

    public void Initialize()
    {
        span = 1.0f;
    }
}
