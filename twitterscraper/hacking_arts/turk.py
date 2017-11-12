from boto.mturk.connection import MTurkConnection
from boto.mturk.question import ExternalQuestion
from boto.mturk.price import Price


from boto.mturk.qualification import Qualifications, Requirement
aws_access_key = 
aws_secret_access_key = 
host_name = 

connection = MTurkConnection(aws_access_key_id=d["aws_key_id"],
                             aws_secret_access_key=d["aws_secret"],
                             host=d["aws_host"])



url = "https://nimbus.media.mit.edu/ishaan//jekyll/update/2016/10/07/" + title + ".html"
    description = "how are you feeling"
    keywords = ["transcription"]
    frame_height = 800
    amount = calculate_amount(title)

    questionform = ExternalQuestion(url, frame_height)

    create_hit_result = connection.create_hit(
            title="transcribe the short video ",
            description=description,
            keywords=keywords,
            max_assignments=1,
            question=questionform,
            reward=Price(amount=amount),
            duration=timedelta(hours=3),
            response_groups=('Minimal', 'HITDetail'),  # I don't know what response groups are
        )
    print create_hit_result[0].HITId