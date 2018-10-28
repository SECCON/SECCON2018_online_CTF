using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.Networking;
using System;

[Serializable]
public class ScoreApiResponse
{
    [SerializeField]
    public RankingRecord[] records;
}

[Serializable]
public class RankingRecord
{
    [SerializeField]
    public int rank;
    public string name;
    public float score;
}

public class GameDirector : MonoBehaviour {
    GameObject scoreLabel;
    GameObject missLabel;
    float score = 0.0f;
    int miss = 0;

    GameObject scoreFormView;
    GameObject rankingView;
    GameObject planeGenerator;

    string apiEndpoint = "http://" + Config.domain;

    public enum STEP
    {
        NONE = -1,
        PLAYING = 0,
        GAMEOVER,
        SENDING_SCORE,
        SENT_SCORE,
        NUM
    }
    public STEP step = STEP.NONE;
    public STEP nextStep = STEP.NONE;

	// Use this for initialization
	void Start () {
        scoreLabel = GameObject.Find("Score");
        missLabel = GameObject.Find("Miss");
        planeGenerator = GameObject.Find("PlaneGenerator");
        scoreFormView = GameObject.Find("ScoreFormView");
        rankingView = GameObject.Find("RankingView");
        scoreFormView.SetActive(false);
        rankingView.SetActive(false);
    }

    void ChangeStep()
    {
        if (this.nextStep != STEP.NONE) return;

        switch (this.step)
        {
            case STEP.NONE:
                nextStep = STEP.PLAYING;
                break;
        }
    }

    void HandleChangingStep()
    {
        while (this.nextStep != STEP.NONE)
        {
            this.step = this.nextStep;
            this.nextStep = STEP.NONE;

            switch(this.step)
            {
                case STEP.GAMEOVER:
                    scoreFormView.SetActive(true);
                    break;
                case STEP.PLAYING:
                    scoreFormView.SetActive(false);
                    rankingView.SetActive(false);
                    miss = 0;
                    score = 0.0f;
                    UpdateScore();
                    UpdateMiss();
                    planeGenerator.GetComponent<PlaneGenerator>().Initialize();
                    planeGenerator.SetActive(true);
                    break;
            }
        }
    }

    void Update () {
        ChangeStep();
        HandleChangingStep();
	}

    public void UpdateScore()
    {
        scoreLabel.GetComponent<Text>().text = Mathf.Floor(this.score).ToString();
    }

    public void UpdateMiss()
    {
        missLabel.GetComponent<Text>().text = this.miss.ToString();
    }

    public void UpdateRanking(string rankingText)
    {
        GameObject.Find("Ranking").GetComponent<Text>().text = rankingText;
    }

    public void AddScore(float score)
    {
        switch(this.step)
        {
            case STEP.PLAYING:
                this.score += score;
                UpdateScore();
                break;
        }
    }

    public void IncrementMiss()
    {
        switch(this.step)
        {
            case STEP.PLAYING:
                if (this.miss < 20)
                    this.miss += 1;
                UpdateMiss();
                if (this.miss >= 20)
                {
                    GameObject.Find("PlaneGenerator").SetActive(false);
                    this.nextStep = STEP.GAMEOVER;
                }
                break;
        }
    }

    public void SubmitScore()
    {
        switch(this.step)
        {
            case STEP.GAMEOVER:
                string name = GameObject.Find("Name").GetComponent<Text>().text;
                if (name != "")
                {
                    // API POST
                    StartCoroutine(PostScore(name, (int)Mathf.Floor(score)));
                    nextStep = STEP.SENDING_SCORE;
                }
                break;
        }
    }

    public void Retry()
    {
        this.nextStep = STEP.PLAYING;
    }

    IEnumerator PostScore(string name, int score)
    {
        WWWForm form = new WWWForm();
        form.AddField("score", score);
        form.AddField("name", name);

        using (UnityWebRequest www = UnityWebRequest.Post(apiEndpoint + "/api/v1/scores", form))
        {
            yield return www.Send();

            if (www.isNetworkError)
            {
                Debug.Log(www.error);
            }
            else
            {
                Debug.Log(www.downloadHandler.text);
                ScoreApiResponse ret = JsonUtility.FromJson<ScoreApiResponse>(www.downloadHandler.text);
                string rankingText = "";
                foreach (var record in ret.records)
                {
                    rankingText += "#" + record.rank.ToString() + " " + record.name + " " + record.score.ToString() + "\n";
                }
                rankingView.SetActive(true);
                UpdateRanking(rankingText);
            }
            scoreFormView.SetActive(false);
            rankingView.SetActive(true);
            nextStep = STEP.SENT_SCORE;
        }
    }
}
