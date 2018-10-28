# frozen_string_literal: true

# == Schema Information
#
# Table name: scores
#
#  id         :bigint(8)        not null, primary key
#  score      :integer
#  name       :string(255)
#  created_at :datetime         not null
#  updated_at :datetime         not null
#

class Score < ApplicationRecord
  include Redis::Objects
  after_save :set_score

  sorted_set 'ranking', global: true

  def self.ranking_around(name)
    nearby_ranks = ranking_nearby(name)
    total = ranking.rank(ranking.last) + 1
    first_rank = total - ranking.rank(nearby_ranks.first[0])
    ret = []
    nearby_ranks.size.times do |i|
      ret.push(
        rank: i + first_rank,
        name: nearby_ranks[i][0],
        score: nearby_ranks[i][1]
      )
    end
    ret
  end

  private

  class << self
    def ranking_nearby(name)
      rank = ranking.rank(name)
      total = ranking.rank(ranking.last) + 1
      ranking.range([rank - 10, 0].max, [rank + 10, total - 1].min, with_scores: true).reverse
    end
  end

  def set_score
    score = Score.ranking.score(name)
    return if score && score > self.score

    Score.ranking[name] = self.score
  end
end
