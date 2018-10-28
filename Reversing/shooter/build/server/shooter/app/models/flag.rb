# == Schema Information
#
# Table name: flags
#
#  id         :bigint(8)        not null, primary key
#  value      :string(255)
#  created_at :datetime         not null
#  updated_at :datetime         not null
#

class Flag < ApplicationRecord
end
