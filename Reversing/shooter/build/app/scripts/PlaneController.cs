using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlaneController : MonoBehaviour {
    Camera mainCamera;
    AudioSource hitAudio;
    GameDirector director;

    public enum STEP {
      NONE = -1,
      FLYING = 0,
      DESTRUCTED,
      WAITING_DESTROY,
      NUM
    }
    public STEP step = STEP.NONE;
    public STEP nextStep = STEP.NONE;
    HitResult lastHitResult;
    float waitingDestructingTimer;

    struct HitResult
    {
        public bool isHit;
        public Vector3 localPoint;
        public Vector3 globalPoint;
    }

	void Start () {
        var d = 3.0f;
        this.transform.position = new Vector3(Random.Range(-d, d), Random.Range(-d, d), 100.0f);
        this.mainCamera = GameObject.Find("Main Camera").GetComponent<Camera>();
        this.hitAudio = GetComponent<AudioSource>();
        this.step = STEP.NONE;
        this.name = System.Guid.NewGuid().ToString();
        director = GameObject.Find("GameDirector").GetComponent<GameDirector>();
	}

    void ChangeStep()
    {
        if (this.nextStep != STEP.NONE) return;

        switch(this.step)
        {
            case STEP.NONE:
                nextStep = STEP.FLYING;
                break;
            case STEP.FLYING:
                HitCheck();
                if (lastHitResult.isHit)
                {
                    nextStep = STEP.DESTRUCTED;
                }
                break;
        }
    }

    void HandleChangingStep()
    {
        while(this.nextStep != STEP.NONE)
        {
            this.step = this.nextStep;
            this.nextStep = STEP.NONE;
            
            switch(this.step)
            {
                case STEP.DESTRUCTED:
                    OnHit();
                    this.transform.position = new Vector3(0.0f, 0.0f, -100.0f);
                    nextStep = STEP.WAITING_DESTROY;
                    waitingDestructingTimer = 2.0f;
                    break;
            }
        }
    }

    void StepRoutine()
    {
        switch(this.step)
        {
            case STEP.FLYING:
                var pos = this.transform.position;
                pos.z = (pos.z + 10.0f) * 0.97f - 10.0f;
                this.transform.position = pos;
                if (!GetComponent<Renderer>().isVisible)
                {
                    director.IncrementMiss();
                    Destroy(gameObject);
                }
                break;
            case STEP.WAITING_DESTROY:
                waitingDestructingTimer -= Time.deltaTime;
                if (waitingDestructingTimer <= 0.0f)
                {
                    Destroy(gameObject);
                }
                break;
        }


    }

    void HitCheck()
    {
        HitResult ret = new HitResult
        {
            isHit = false
        };

        if (Input.GetMouseButtonDown(0))
        {
            RaycastHit hit;
            Ray ray = mainCamera.ScreenPointToRay(Input.mousePosition);

            if (Physics.Raycast(ray, out hit))
            {
                if (hit.collider.gameObject.name == this.name)
                {
                    var localPoint = this.transform.InverseTransformPoint(hit.point);
                    ret.isHit = true;
                    ret.localPoint = localPoint;
                    ret.globalPoint = hit.point;
                    lastHitResult = ret;
                }
            }
        }
    }
	
	void Update () {
        ChangeStep();
        HandleChangingStep();
        StepRoutine();
    }

    void OnHit()
    {
        var localPoint = lastHitResult.localPoint;
        var globalPoint = lastHitResult.globalPoint;
        var score = (10.0f - Mathf.Abs(localPoint.x) - Mathf.Abs(localPoint.z)) * ((globalPoint.z + 10.0f) / 10.0f);
        //Debug.Log(localPoint.ToString() + "/" + globalPoint.ToString() + "/" + score);

        hitAudio.Play();

        director.AddScore(score);
    }
}
