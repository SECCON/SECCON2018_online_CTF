class CreateManagers < ActiveRecord::Migration[5.2]
  def change
    create_table :managers do |t|
      t.string :login_id
      t.string :password_hash

      t.timestamps
    end
  end
end
