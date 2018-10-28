# frozen_string_literal: true

# == Schema Information
#
# Table name: managers
#
#  id            :bigint(8)        not null, primary key
#  login_id      :string(255)
#  password_hash :string(255)
#  created_at    :datetime         not null
#  updated_at    :datetime         not null
#

class Manager < ApplicationRecord
end
