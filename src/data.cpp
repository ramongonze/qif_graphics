#include "data.h"

Data::Data(){
	hyperReady = false;
    mouseClickedOnPrior = false;
    prior = vector<long double>(NUMBER_SECRETS, 0);
    channel = vector<vector<long double>>(NUMBER_SECRETS, vector<long double>(MAX_CHANNEL_OUTPUTS, 0));
    error = NO_ERROR;
    fileSaved = true;
    
    // Calculates the number of frames the animation will have, considering the software is running in 60 FPS
    animation = -1;
    animationRunning = false;
}

int Data::checkPriorText(char prior_[NUMBER_SECRETS][CHAR_BUFFER_SIZE]){
    vector<pair<string, string>> newPrior(NUMBER_SECRETS);
    vector<string> priorStr(NUMBER_SECRETS);

    for(int i = 0; i < NUMBER_SECRETS; i++){
    	priorStr[i] = string(prior_[i]);
    }

    try{
        for(int i = 0; i < NUMBER_SECRETS; i++){
            size_t pos = priorStr[i].find('/');
            if(pos != string::npos){ // If true, the user is typing a fraction
                string numerator = priorStr[i].substr(0, pos);
                string denominator = priorStr[i].substr(pos+1, priorStr[i].size()-pos-1);
                
                // Remove blank spaces
                numerator.erase(remove(numerator.begin(), numerator.end(), ' '), numerator.end());
                denominator.erase(remove(denominator.begin(), denominator.end(), ' '), denominator.end());
                
                newPrior[i] = make_pair(numerator, denominator);
            }else{
            	newPrior[i] = make_pair("not fraction", priorStr[i]);
            }
        }
        
        // Update values
        this->prior = vector<long double>(NUMBER_SECRETS);
        for(int i = 0; i < NUMBER_SECRETS; i++){
        	if(newPrior[i].first == "not fraction"){
        		this->prior[i] = std::stold(newPrior[i].second);
        	}else{
        		this->prior[i] = std::stold(newPrior[i].first)/std::stold(newPrior[i].second);
        	}
        }
        
        return NO_ERROR;
    }catch(exception& e){
        return INVALID_VALUE;
    }
}

int Data::checkChannelText(char channel_[NUMBER_SECRETS][MAX_CHANNEL_OUTPUTS][CHAR_BUFFER_SIZE], int numOutputs){
    vector<vector<pair<string, string>>> newChannel(NUMBER_SECRETS, vector<pair<string, string>>(numOutputs));
    vector<vector<string>> channelStr(NUMBER_SECRETS, vector<string>(numOutputs));

    for(int i = 0; i < NUMBER_SECRETS; i++){
    	for(int j = 0; j < numOutputs; j++){
    		channelStr[i][j] = string(channel_[i][j]);
    	}
    }

    try{
        for(int i = 0; i < NUMBER_SECRETS; i++){
            for(int j = 0; j < numOutputs; j++){
                size_t pos = channelStr[i][j].find('/');
                if(pos != string::npos){ // If true, the user is typing a fraction
                    string numerator = channelStr[i][j].substr(0, pos);
                    string denominator = channelStr[i][j].substr(pos+1, channelStr[i][j].size()-pos-1);
                    
                    // Remove blank spaces
                    numerator.erase(remove(numerator.begin(), numerator.end(), ' '), numerator.end());
                    denominator.erase(remove(denominator.begin(), denominator.end(), ' '), denominator.end());

                    newChannel[i][j] = make_pair(numerator, denominator);
                }else{
                    newChannel[i][j] = make_pair("not fraction", channelStr[i][j]);
                }
            }
        }

        // Update values. Columns and rows are inverted in channelStr.
        this->channel = vector<vector<long double>>(NUMBER_SECRETS, vector<long double>(numOutputs));
        for(int i = 0; i < NUMBER_SECRETS; i++){
        	for(int j = 0; j < numOutputs; j++){
        		if(newChannel[i][j].first == "not fraction"){
        			this->channel[i][j] = std::stold(newChannel[i][j].second);
        		}else{
        			this->channel[i][j] = std::stold(newChannel[i][j].first)/std::stold(newChannel[i][j].second);
        		}
        	}
        }

        return NO_ERROR;
    }catch(exception& e){
        return INVALID_VALUE;
    }
}

void Data::buildCircles(Vector2 TrianglePoints[3]){
    Point p;

    if(animationRunning && animation == STEPS){
        // First draw of the animation

        // Prior
        p = dist2Bary(hyper.prior);
        p = bary2Pixel(p.x, p.y, TrianglePoints);    
        priorCircle.center = Point(p.x, p.y);
        priorCircle.radius = PRIOR_RADIUS;

        for(int i = 0; i < hyper.num_post; i++){
            p = dist2Bary(hyper.inners[0][i], hyper.inners[1][i], hyper.inners[2][i]);
            p = bary2Pixel(p.x, p.y, TrianglePoints);
            // All inners starts in prior position
            innersCircles[i].center = priorCircle.center;
            innersCircles[i].radius = (int)sqrt(hyper.outer.prob[i] * PRIOR_RADIUS * PRIOR_RADIUS);
            
            long double deltaX = p.x - priorCircle.center.x;
            long double deltaY = p.y - priorCircle.center.y;
            xJumpAnimation[i] = deltaX/STEPS;
            yJumpAnimation[i] = deltaY/STEPS;
        }
        
        animation--;
    }else if(animationRunning && animation > 0){
        for(int i = 0; i < hyper.num_post; i++){
            innersCircles[i].center.x += xJumpAnimation[i];
            innersCircles[i].center.y += yJumpAnimation[i];
        }
        animation--;
    }else if((animationRunning && animation == 0) || animation == UPDATE_CIRCLES_BY_MOUSE){
        // Prior
        p = dist2Bary(hyper.prior);
        p = bary2Pixel(p.x, p.y, TrianglePoints);    
        priorCircle.center = Point(p.x, p.y);
        priorCircle.radius = PRIOR_RADIUS;
        
        for(int i = 0; i < hyper.num_post; i++){
            p = dist2Bary(hyper.inners[0][i], hyper.inners[1][i], hyper.inners[2][i]);
            p = bary2Pixel(p.x, p.y, TrianglePoints);
            innersCircles[i].center = Point(p.x, p.y);
            innersCircles[i].radius = (int)sqrt(hyper.outer.prob[i] * PRIOR_RADIUS * PRIOR_RADIUS);
        }
        animationRunning = false;
    }
}

int Data::orientation(Point p1, Point p2, Point p3){
    int val = (p2.y - p1.y) * (p3.x - p2.x) - 
              (p2.x - p1.x) * (p3.y - p2.y); 
  
    if (val == 0) return 0;  // colinear 
  
    if(val >= 0) return 1; // clock wise
    else return 2; // counterclock wise
}

Point Data::pointIntersection(Point A, Point B, Point C, Point D){
    // Line AB represented as a1x + b1y = c1 
    long double a1 = B.y - A.y; 
    long double b1 = A.x - B.x; 
    long double c1 = a1*(A.x) + b1*(A.y); 
  
    // Line CD represented as a2x + b2y = c2 
    long double a2 = D.y - C.y; 
    long double b2 = C.x - D.x; 
    long double c2 = a2*(C.x)+ b2*(C.y); 
  
    long double determinant = a1*b2 - a2*b1; 
  
    Point p = Point((b2*c1 - b1*c2)/determinant, (a1*c2 - a2*c1)/determinant);

    return p; 
}

Point Data::adjustPrior(Vector2 TrianglePoints[3], Vector2 mouse){
    Point mousePosition(mouse.x, WINDOWS_HEIGHT - mouse.y);
    Point TP0(TrianglePoints[0].x, WINDOWS_HEIGHT - TrianglePoints[0].y);
    Point TP1(TrianglePoints[1].x, WINDOWS_HEIGHT - TrianglePoints[1].y);
    Point TP2(TrianglePoints[2].x, WINDOWS_HEIGHT - TrianglePoints[2].y);

    int oL = orientation(TP1, mousePosition, TP0); // Left edge
    int oR = orientation(TP2, mousePosition, TP0); // Right edge
    int oD = orientation(TP1, mousePosition, TP2); // Down edge

    // Check if the mosuePoint is colinear with one of the triangle edges.
    if(oL == 0){
        if(mousePosition.y > TP0.y){
            TP0.y = WINDOWS_HEIGHT - TP0.y;
            return TP0;
        }else if(mousePosition.y < TP1.y){
            TP1.y = WINDOWS_HEIGHT - TP1.y;
            return TP1;
        }else{
            mousePosition.y = WINDOWS_HEIGHT - mousePosition.y;
            return mousePosition;
        }
    }

    if(oR == 0){
        if(mousePosition.y > TP0.y){
            TP0.y = WINDOWS_HEIGHT - TP0.y;
            return TP0;
        }else if(mousePosition.y < TP2.y){
            TP2.y = WINDOWS_HEIGHT - TP2.y;
            return TP2;
        }else{
            mousePosition.y = WINDOWS_HEIGHT - mousePosition.y;
            return mousePosition;
        }
    }

    if(oD == 0){
        if(mousePosition.x < TP1.x){
            TP1.y = WINDOWS_HEIGHT - TP1.y;
            return TP1;
        }else if(mousePosition.x > TP2.x){
            TP2.y = WINDOWS_HEIGHT - TP2.y;
            return TP2;
        }else{
            mousePosition.y = WINDOWS_HEIGHT - mousePosition.y;
            return mousePosition;
        }
    }

    /*
     *            /\
     * Region 1  /  \ Region 2
     *          /    \
     *         /______\
     *         Region 3
     *
     * Region 1: Above the left edge
     * Region 2: Above the right edge
     * Region 3: Below the down edge
     *
    */

    bool isInRegion1, isInRegion2, isInRegion3;

    if(oL == 1) isInRegion1 = true;
    else isInRegion1 = false;

    if(oR == 2) isInRegion2 = true;
    else isInRegion2 = false;

    if(oD == 2) isInRegion3 = true;
    else isInRegion3 = false;

    if(!isInRegion1 && !isInRegion2 && !isInRegion3){
        mousePosition.y = WINDOWS_HEIGHT - mousePosition.y;
        return mousePosition;
    }

    if(isInRegion1 && isInRegion2){
        TP0.y = WINDOWS_HEIGHT - TP0.y;
        return TP0;
    }
    if(isInRegion2 && isInRegion3){
        TP2.y = WINDOWS_HEIGHT - TP2.y;
        return TP2;
    }
    if(isInRegion1 && isInRegion3){
        TP1.y = WINDOWS_HEIGHT - TP1.y;
        return TP1;
    }

    Point p;

    if(isInRegion1) p = pointIntersection(TP0, TP1, mousePosition, TP2);
    if(isInRegion2) p = pointIntersection(TP0, TP2, TP1, mousePosition);
    if(isInRegion3) p = pointIntersection(TP1, TP2, mousePosition, TP0);

    p.y = WINDOWS_HEIGHT - p.y;
    return p;
}

void Data::updateHyper(Vector2 TrianglePoints[3]){
    
    Point mousePosition;
    vector<long double> newPrior(NUMBER_SECRETS);

    mousePosition = adjustPrior(TrianglePoints, GetMousePosition());
    mousePosition = pixel2Bary(mousePosition.x, mousePosition.y, TrianglePoints);
    
    bary2Dist(mousePosition, newPrior);
    Distribution D(newPrior);
    hyper.rebuildHyper(D);
    this->prior = vector<long double>({newPrior[0], newPrior[1], newPrior[2]});
}

void Data::newRandomPrior(){
    srand(unsigned(time(0)));

    int threshold = 100, p;
    for(int i = 0; i < 2; i++){
        p = rand() % threshold;
        threshold -= p;
        prior[i] = p/100.0;
    }
    prior[2] = threshold/100.0;

    random_shuffle(prior.begin(), prior.end());
}

void Data::newRandomChannel(int num_out){
    srand(unsigned(time(0)));
    vector<long double> prob(num_out);
    this->channel = vector<vector<long double>>(NUMBER_SECRETS, vector<long double>(num_out, 0));

    for(int i = 0; i < NUMBER_SECRETS; i++){
        int threshold = 100, p;
        for(int j = 0; j < num_out-1; j++){
            p = rand() % threshold;
            threshold -= p;
            prob[j] = p/100.0;
        }
        prob[num_out-1] = threshold/100.0;

        random_shuffle(prob.begin(), prob.end());

        for(int j = 0; j < num_out; j++){
            channel[i][j] = prob[j];
        }
    }
}
