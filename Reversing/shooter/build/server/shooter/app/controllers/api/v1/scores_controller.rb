# frozen_string_literal: true

class Api::V1::ScoresController < ApiController
  def create
    return if params[:name].blank?

    params[:name] = params[:name].to_s
    params[:name] = params[:name][0, [params[:name].length, 12].min]
    Score.create(name: params[:name], score: params[:score])
    render json: { records: Score.ranking_around(params[:name]) }
  end
end
