if Manager.count.zero?
  Manager.create(login_id: 'admin', password_hash: Digest::MD5.hexdigest('Xc8LmM5T'))
end

if Rails.env.staging? && Flag.count.zero?
  Flag.create(value: 'SECCON{1NV4L1D_4DM1N_P4G3_4U+H3NT1C4T10N}')
end
